#pragma once
#include "defines.hpp"
#include "TS/queue.hpp"
#include "Messages/message.hpp"
#include "Context.hpp"
#include "UDP/packet.hpp"
#include "UDP/udp.hpp"
#include "UDP/Buffer/BufferManager.hpp"
#include "UDP/ReuseablePacket.hpp"
#include "UDP/PacketWrapperChecker.hpp"
#include "TCP/Connection.hpp"
#include "ClientServerBase/enums.hpp"

namespace JNet {

    

    
    template<TemplatedServerArgs>
    class Server : udp::PacketWrapperChecker<TPacketWrapper> {
    public:
        using ReuseableBuffer = udp::ReuseableBuffer<udp::bufferSize,true>;
        using BufferManager = udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, true>;
        using ReuseablePacket = udp::ReuseablePacket<TPacketWrapper ,udp::bufferSize, true>;
        using Connection = tcp::Connection;
        
    public:
        Server(uint16_t port);
        void run();
        void close(std::chrono::microseconds finishTime = std::chrono::microseconds(100));
        /** gets and empty packet which can be send using this server
        */        
        ReuseablePacket getEmptyPacket();
        void sendPacket(ReuseablePacket packet);
        //void setPacketCallback(void (*callback) ()); //uhhh yeah that's not how programming works
        bool hasAvailablePacket();
        ReuseablePacket receiveIncomingPacket();
        void returnPacket(ReuseablePacket packet);
        bool isRunning();
        //void setPacketCallback(std::function<void()> callback);
        ~Server();
    private:
        void acceptConnections();
        void acceptConnection();
        void handleConnectionAccept(Connection* connection, const boost::system::error_code& e);

        void receivePackets();
        void receivePacket();

        void handlePacketReceive(ReuseableBuffer* recycleableBuffer ,const boost::system::error_code& e, size_t messageSize);
        void handleSentPacket(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize);
        void sendNextPacket(); 
    private:
        //boost::asio::ip::tcp::acceptor acceptor;
        //std::function<void()> tcpCallback;
        
        JNet::Context context;
        bool shouldClose = false;

        //std::function<void()> udpCallback;
        BufferManager bufferManager;
        udp::Socket udpSocket;
        udp::Endpoint udpEndpoint;
        JNet::ts::Queue<ReuseableBuffer*> incomingPackets;
        JNet::ts::Queue<ReuseableBuffer*> outgoingPackets;
        boost::asio::thread_pool udpSender{1};


    };

    template<TemplatedServerArgs>
    void TemplatedServer::handleConnectionAccept(Connection* connection, const boost::system::error_code& e) {
        acceptConnection();




    }


    template<TemplatedServerArgs>
    TemplatedServer::Server(uint16_t port) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
        //, acceptor(context.getAsioContext(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) 
        {
        //boost::asio::ip::tcp::endpoint endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
        //acceptor.open(endpoint.protocol());
        //acceptor.bind(endpoint);
        //acceptor.listen();
        //std::cout << "Constructed server\n";
    }

    template<TemplatedServerArgs>
    void TemplatedServer::run() {
        context.async_run();
        receivePackets();
        acceptConnections();
    }

    template<TemplatedServerArgs>
    void TemplatedServer::acceptConnections() { 
        acceptConnection();
    }

    template<TemplatedServerArgs>
    void TemplatedServer::receivePackets() { 
        //std::cout << "Starting receiving packets\n";
        receivePacket();
    }

    template<TemplatedServerArgs>
    void TemplatedServer::acceptConnection() { 
        //Connection* openConnection = Connection::create(context);

        //auto callback = boost::bind(&Server::handleConnectionAccept, this, openConnection, boost::asio::placeholders::error);

        //acceptor.async_accept(openConnection->getSocket(), callback);
    }



    template<TemplatedServerArgs>
    void TemplatedServer::receivePacket() {
        ReuseableBuffer* buffer = bufferManager.getBuffer();
        auto callback = boost::bind(&Server::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred);

        udpSocket.async_receive_from(
        boost::asio::buffer(buffer->buffer), 
        buffer->endpoint, 
        callback
        );
    }

    template<TemplatedServerArgs>
    void TemplatedServer::handlePacketReceive(ReuseableBuffer* recycleableBuffer,const boost::system::error_code& e, size_t messageSize) {
        if (shouldClose) {
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Unexecuted receive due to server closing\n";
            bufferManager.recycleBuffer(recycleableBuffer);
            return;
        }
        receivePacket();
        if (e.failed()) {
            std::stringstream ss;
            ss << "Error when handling packet Receive:\n" << e.message() << "\n";
            std::cerr << ss.str();
            bufferManager.recycleBuffer(recycleableBuffer);
            return;
        }
        
        
        if (debugFlagActive<DebugFlag::serverDebug>()) 
            std::cout << "Handling received packet" << "\n";


        
        //if (debugFlagActive<DebugFlag::serverDebug>()) 
        //    std::cout << recycleableBuffer->packet().debugString();
        incomingPackets.push(recycleableBuffer);
        
    }

    template<TemplatedServerArgs>
    void TemplatedServer::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize) {
        if (e.failed()) {
            std::stringstream ss;
            ss << "Error when responding:\n" << e.message() << "\n";
            std::cerr << ss.str();     
        }
        if (debugFlagActive<DebugFlag::serverDebug>()) 
            std::cout << "Successfully responded!\n";
        bufferManager.recycleBuffer(recycleableBuffer);
    }

    template<TemplatedServerArgs>
    void TemplatedServer::sendNextPacket() {
        ReuseableBuffer* sendBuffer = outgoingPackets.consumeFront();
        auto callBack = boost::bind(
            &Server::handleSentPacket
            ,this
            ,sendBuffer
            ,boost::asio::placeholders::error()
            ,boost::asio::placeholders::bytes_transferred()
        );
        udpSocket.async_send_to(boost::asio::buffer(sendBuffer->buffer),sendBuffer->endpoint,callBack);
    }

    template<TemplatedServerArgs>
    void TemplatedServer::close(std::chrono::microseconds finishTime) {
        shouldClose = true;
        context.shutDown(finishTime);
        outgoingPackets.clear();
        incomingPackets.clear();
        udpSender.join();
    }

    template<TemplatedServerArgs>
    typename TemplatedServer::ReuseablePacket TemplatedServer::getEmptyPacket() {
        return ReuseablePacket(bufferManager.getBuffer());
    }

    template<TemplatedServerArgs>
    void TemplatedServer::sendPacket(ReuseablePacket packet) {
        outgoingPackets.push(packet.buffer);

        boost::asio::post(udpSender, boost::bind(&Server::sendNextPacket,this));
    }

    //template<TemplatedServerArgs>
    //void TemplatedServer::setPacketCallback(void (*callback)()) {
        //this->callback = callback;
    //}

    template<TemplatedServerArgs>
    bool TemplatedServer::hasAvailablePacket() {
        return !incomingPackets.empty();
    }

    template<TemplatedServerArgs>
    typename TemplatedServer::ReuseablePacket JNet::TemplatedServer::receiveIncomingPacket() {
        return ReuseablePacket(incomingPackets.consumeFront());
    }

    template<TemplatedServerArgs>
    void TemplatedServer::returnPacket(ReuseablePacket packet) {
        bufferManager.recycleBuffer(packet.buffer);
    }

    template<TemplatedServerArgs>
    bool TemplatedServer::isRunning() {
        return !shouldClose;
    }

    //template <TemplatedServerArgs>
    //inline void TemplatedServer::setPacketCallback(std::function<void()> callback) {
        //udpCallback = callback;
    //}

    template<TemplatedServerArgs>
    TemplatedServer::~Server() {
        if (!shouldClose) {
            std::cerr << "You should call close() before destroying a server object\n";
            close();
        }
    }
}