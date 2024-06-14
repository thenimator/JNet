#pragma once
#include "defines.hpp"
#include "TS/queue.hpp"
#include "Messages/message.hpp"
#include "Context.hpp"
#include "UDP/packet.hpp"
#include "UDP/udp.hpp"
#include "UDP/Buffer/BufferManager.hpp"
#include "UDP/ReuseablePacket.hpp"


namespace JNet {

        

    class Server {
    public:
        using ReuseableBuffer = udp::ReuseableBuffer<udp::bufferSize,true>;
        using BufferManager = udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, true>;
        using ReuseablePacket = udp::ReuseablePacket<udp::bufferSize, true>;
        using Packet = udp::Packet<udp::bufferSize>;
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
    
    
}