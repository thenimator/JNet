#pragma once
#include "TS/queue.hpp"
#include "defines.hpp"
#include "TS/ts.hpp"
#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/asio/placeholders.hpp>
#include <type_traits>
#include "Context.hpp"
#include "UDP/ReuseablePacket.hpp"
#include "UDP/udp.hpp"
#include "UDP/PacketWrapperChecker.hpp"


namespace JNet {
    //TODO: Client and Server inherit from shared base class
    //TODO: Context always has to call terminate
    template<TemplatedClientArgs>
    class Client : udp::PacketWrapperChecker<TPacketWrapper> {
    public:
        using ReuseableBuffer = udp::ReuseableBuffer<udp::bufferSize,false>;
        using BufferManager = udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, false>;
        using ReuseablePacket = udp::ReuseablePacket<TPacketWrapper, udp::bufferSize, false>;
        
    public:
        //requires its own context
        Client();
        Client(const Client&) = delete;
        TemplatedClient& operator=(TemplatedClient&) = delete;
        ~Client();
        ReuseablePacket getPacket();
        void sendPacket(ReuseablePacket packet);
        void connect(const std::string& host, const std::string& port);
        /** @brief closes the currently active connection to a server
         * @brief guaranteed to wait at least finishDuration for outstanding operations
         */
        void disconnect(std::chrono::microseconds finishDuration = std::chrono::microseconds(100));
        bool hasAvailablePacket();
        ReuseablePacket receiveIncomingPacket();
        void returnPacket(ReuseablePacket packet);
        bool hasConnection();
    private:
        void receive();
        void handlePacketReceive(ReuseableBuffer* recycleableBuffer ,const boost::system::error_code& e, size_t messageSize);
    private:
        bool shouldDisconnect = true;
        Context context;
        std::string host = "";
        std::string port = "";
        uint64_t udpMessageCount = 0;


        boost::asio::thread_pool udpSender{1};
        udp::Socket udpSocket;
        udp::Endpoint udpEndpoint;
        udp::Resolver udpResolver;
        ts::Queue<ReuseableBuffer*> incomingPackets;
        ts::Queue<ReuseableBuffer*> outgoingPackets;
        BufferManager bufferManager;



    private:
        void resolveHost();
        void handleSentPacket(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize);
        void sendNextPacketToHost(); 

        
    };


    template<TemplatedClientArgs>
    typename TemplatedClient::ReuseablePacket TemplatedClient::getPacket() {
        return ReuseablePacket(bufferManager.getBuffer());
    }

    template<TemplatedClientArgs>
    TemplatedClient::Client() : udpSocket(context.getAsioContext()), udpResolver(context.getAsioContext()) {

    }

    template<TemplatedClientArgs>
    TemplatedClient::~Client() {
        if (!shouldDisconnect)  {
            std::cerr << "You should call disnonnect() before destroying a client object\n";
        }
    }

    template<TemplatedClientArgs>
    void TemplatedClient::sendPacket(TemplatedClient::ReuseablePacket packet) {
        if (host == "") {
            if (debugFlagActive<DebugFlag::clientDebug>()) 
                std::cout << "No host given but packet send attempted\n";
            bufferManager.recycleBuffer(packet.buffer);

            return;
        }
        outgoingPackets.push(packet.buffer);

        boost::asio::post(udpSender, boost::bind(&Client::sendNextPacketToHost,this));
    }

    template<TemplatedClientArgs>
    void TemplatedClient::connect(const std::string &host, const std::string& port) {
        shouldDisconnect = false;
        context.async_run();
        this->host = host;
        this->port = port;
        resolveHost();
        try {
            udpSocket.connect(udpEndpoint);
            if (debugFlagActive<DebugFlag::clientDebug>()) 
                std::cout << "Conntected with: " <<
                udpEndpoint << "\n";
        } catch (boost::system::system_error& e) {
            std::cerr << e.what() << std::endl;
        }
        receive();
    }

    template<TemplatedClientArgs>
    void TemplatedClient::disconnect(std::chrono::microseconds finishDuration) {
        shouldDisconnect = true;
        context.shutDown(finishDuration);
        incomingPackets.clear();
        outgoingPackets.clear();
        udpSender.join();
    }

    template<TemplatedClientArgs>
    bool TemplatedClient::hasAvailablePacket() {
        return !incomingPackets.empty();
    }

    template<TemplatedClientArgs>
    typename TemplatedClient::ReuseablePacket TemplatedClient::receiveIncomingPacket() {
        return ReuseablePacket(incomingPackets.consumeFront());
    }

    template<TemplatedClientArgs>
    void TemplatedClient::returnPacket(ReuseablePacket packet) {
        bufferManager.recycleBuffer(packet.buffer);
    }

    template<TemplatedClientArgs>
    bool TemplatedClient::hasConnection() {
        return !shouldDisconnect;
    }

    template<TemplatedClientArgs>
    void TemplatedClient::receive() {
        ReuseableBuffer* buffer = bufferManager.getBuffer();
        if (debugFlagActive<DebugFlag::clientDebug>()) {
            std::stringstream ss;
            ss << "Using buffer with id: " << buffer << " as receiveBuffer\n";
            std::cout << ss.str();
        }
        auto callback = boost::bind(&Client::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred);

        udpSocket.async_receive(
        boost::asio::buffer(buffer->buffer), 
        callback
        );
    }

    template<TemplatedClientArgs>
    void TemplatedClient::handlePacketReceive(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, size_t messageSize) {
        if (debugFlagActive<DebugFlag::clientDebug>()) {
            std::stringstream ss;
            ss << "Started handling receive for " << messageSize << " bytes\n";
            std::cout << ss.str();
        }

        /*if (shouldClose) {
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Unexecuted receive due to server closing\n";
            bufferManager.recycleBuffer(recycleableBuffer);
            return;
        }*/
        receive();
        if (e.failed()) {
            std::cerr << "Error:\n" << e.message() << "\n";
            bufferManager.recycleBuffer(recycleableBuffer);
            return;
        }
        incomingPackets.push(recycleableBuffer);
    }

    template<TemplatedClientArgs>
    void TemplatedClient::resolveHost() {
        try {
            udpEndpoint = *udpResolver.resolve(host,port).begin();
        } catch (boost::system::system_error& e) {
            std::cout << "Error when connecting with " << host << std::endl;
            std::cerr << e.what() << std::endl;
            std::cerr << "Code: " << e.code() << std::endl;
        }
    }

    template<TemplatedClientArgs>
    void TemplatedClient::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize) {
        bufferManager.recycleBuffer(recycleableBuffer);
        if (e.failed()) {
            std::cerr << "Sending failed. Code: " << e.value() << "\n";
        }
    }

    template<TemplatedClientArgs>
    void TemplatedClient::sendNextPacketToHost() {
        ReuseableBuffer* sendBuffer = outgoingPackets.consumeFront();
        auto callBack = boost::bind(
            &Client::handleSentPacket
            ,this
            ,sendBuffer
            ,boost::asio::placeholders::error()
            ,boost::asio::placeholders::bytes_transferred()
        );
        udpSocket.async_send(boost::asio::buffer(sendBuffer->buffer),callBack);
        if (debugFlagActive<DebugFlag::clientDebug>()) {
            std::stringstream ss;

            ss << "Sent to: " << udpSocket.remote_endpoint() << "\n";
        
            std::cout << ss.str();
        }
        bufferManager.recycleBuffer(sendBuffer);
    }

    
}