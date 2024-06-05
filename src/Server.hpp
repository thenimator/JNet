#pragma once
#include "defines.hpp"
#include "TS/queue.hpp"
#include "Messages/message.hpp"
#include "Context.hpp"
#include "UDP/packet.hpp"
#include "UDP/udp.hpp"
#include "UDP/Buffer/BufferManager.hpp"


namespace JNet {

        

    class Server {
    public:
        typedef udp::ReuseableBuffer<udp::bufferSize,true> ReuseableBuffer;
        typedef udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, true> BufferManager;
    public:
        Server() = delete;
        Server(JNet::Context& context);
        void run();
        void close();
        ~Server();
    private:
        
        void receive();
        void respond(const boost::system::error_code& e, size_t messageSize);
        void handleMessage(const boost::system::error_code& e, size_t messageSize);
        void handlePacketReceive(ReuseableBuffer* recycleableBuffer ,const boost::system::error_code& e, size_t messageSize);
    private:

        
        bool shouldClose = false;
        uint64_t messageCount = 0;

        BufferManager bufferManager;
        udp::Socket udpSocket;
        udp::Endpoint udpEndpoint;
        JNet::ts::Queue<udp::Packet<>> incomingPackets;

    };
    
    
}