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


namespace JNet {

        
    template<class TPacketWrapper>
    class Server : udp::PacketWrapperChecker<TPacketWrapper> {
    public:
        using ReuseableBuffer = udp::ReuseableBuffer<udp::bufferSize,true>;
        using BufferManager = udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, true>;
        using ReuseablePacket = udp::ReuseablePacket<TPacketWrapper ,udp::bufferSize, true>;
    public:
        Server(uint16_t port);
        void run();
        void close(std::chrono::microseconds finishTime = std::chrono::microseconds(100));
        /** gets and empty packet which can be send using this server
        */        
        ReuseablePacket getEmptyPacket();
        void sendPacket(ReuseablePacket packet);
        void setPacketCallback(void (*callback) ()); //uhhh yeah that's not how programming works
        bool hasAvailablePacket();
        ReuseablePacket receiveIncomingPacket();
        void returnPacket(ReuseablePacket packet);
        bool isRunning();
        ~Server();
    private:
        
        void receive();
        void handlePacketReceive(ReuseableBuffer* recycleableBuffer ,const boost::system::error_code& e, size_t messageSize);
        void handleSentPacket(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize);
        void sendNextPacket(); 
    private:

        
        JNet::Context context;
        bool shouldClose = false;

        void (*callback) () = [](){};
        BufferManager bufferManager;
        udp::Socket udpSocket;
        udp::Endpoint udpEndpoint;
        JNet::ts::Queue<ReuseableBuffer*> incomingPackets;
        JNet::ts::Queue<ReuseableBuffer*> outgoingPackets;
        boost::asio::thread_pool udpSender{1};


    };


    template<class TPacketWrapper>
    Server<TPacketWrapper>::Server(uint16_t port) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {

    }

    template<class TPacketWrapper>
    void Server<TPacketWrapper>::run() {
        receive();
        context.async_run();
    }

    template<class TPacketWrapper>
    void Server<TPacketWrapper>::receive() {
        ReuseableBuffer* buffer = bufferManager.getBuffer();
        auto callback = boost::bind(&Server::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred);

        udpSocket.async_receive_from(
        boost::asio::buffer(buffer->buffer), 
        buffer->endpoint, 
        callback
        );
    }

    template<class TPacketWrapper>
    void Server<TPacketWrapper>::handlePacketReceive(ReuseableBuffer* recycleableBuffer,const boost::system::error_code& e, size_t messageSize) {
        if (shouldClose) {
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Unexecuted receive due to server closing\n";
            bufferManager.recycleBuffer(recycleableBuffer);
            return;
        }
        receive();
        if (e.failed()) {
            std::cerr << "Error:\n" << e.message() << "\n";
            bufferManager.recycleBuffer(recycleableBuffer);
            return;
        }
        
        
        if (debugFlagActive<DebugFlag::serverDebug>()) 
            std::cout << "Handling received packet" << "\n";


        
        //if (debugFlagActive<DebugFlag::serverDebug>()) 
        //    std::cout << recycleableBuffer->packet().debugString();
        incomingPackets.push(recycleableBuffer);
        
    }

    template<class TPacketWrapper>
    void Server<TPacketWrapper>::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize) {
        if (e.failed()) {
            std::stringstream ss;
            ss << "Error when responding:\n" << e.message() << "\n";
            std::cerr << ss.str();     
        }
        if (debugFlagActive<DebugFlag::serverDebug>()) 
            std::cout << "Successfully responded!\n";
        bufferManager.recycleBuffer(recycleableBuffer);
    }

    template<class TPacketWrapper>
    void Server<TPacketWrapper>::sendNextPacket() {
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

    template<class TPacketWrapper>
    void Server<TPacketWrapper>::close(std::chrono::microseconds finishTime) {
        shouldClose = true;
        context.shutDown(finishTime);
        outgoingPackets.clear();
        incomingPackets.clear();
        udpSender.join();
    }

    template<class TPacketWrapper>
    typename Server<TPacketWrapper>::ReuseablePacket Server<TPacketWrapper>::getEmptyPacket() {
        return ReuseablePacket(bufferManager.getBuffer());
    }

    template<class TPacketWrapper>
    void Server<TPacketWrapper>::sendPacket(ReuseablePacket packet) {
        outgoingPackets.push(packet.buffer);

        boost::asio::post(udpSender, boost::bind(&Server::sendNextPacket,this));
    }

    template<class TPacketWrapper>
    void Server<TPacketWrapper>::setPacketCallback(void (*callback)()) {
        this->callback = callback;
    }

    template<class TPacketWrapper>
    bool Server<TPacketWrapper>::hasAvailablePacket() {
        return !incomingPackets.empty();
    }

    template<class TPacketWrapper>
    typename Server<TPacketWrapper>::ReuseablePacket JNet::Server<TPacketWrapper>::receiveIncomingPacket() {
        return ReuseablePacket(incomingPackets.consumeFront());
    }

    template<class TPacketWrapper>
    void Server<TPacketWrapper>::returnPacket(ReuseablePacket packet) {
        bufferManager.recycleBuffer(packet.buffer);
    }

    template<class TPacketWrapper>
    bool Server<TPacketWrapper>::isRunning() {
        return !shouldClose;
    }

    template<class TPacketWrapper>
    Server<TPacketWrapper>::~Server() {
        if (!shouldClose) {
            std::cerr << "You should call close() before destroying a server object\n";
            close();
        }
    }
    
    
}