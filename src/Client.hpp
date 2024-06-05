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


namespace JNet {
    class Client {
    public:
        typedef udp::ReuseableBuffer<udp::bufferSize,false> ReuseableBuffer;
        typedef udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, false> BufferManager;
        typedef udp::ReuseablePacket<udp::bufferSize, false> ReuseablePacket;
        typedef udp::Packet<udp::bufferSize, false> Packet;
        
    public:
        Client(Context& givenContext);
        Client(const Client&) = delete;
        Client& operator=(Client&) = delete;
        ~Client();
        udp::ReuseablePacket<> getPacket();
        void sendPacket(udp::ReuseablePacket<> packet);
        void connect(const std::string& host);
        std::unique_ptr<udp::Packet<>> receiveMessage();
    private:
        Context& context;
        std::string host = "";
        uint64_t udpMessageCount = 0;


        boost::asio::thread_pool udpSender{1};
        udp::Socket udpSocket;
        udp::Endpoint udpEndpoint;
        udp::Resolver udpResolver;
        std::thread udpReceiver;
        ts::Queue<std::unique_ptr<Packet>> incomingPackets;
        ts::Queue<ReuseableBuffer*> outgoingPackets;
        BufferManager bufferManager;



    private:
        void resolveHost();
        void handleSentPacket(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize);
        void sendNextPacketToHost(); 

        
    };

    
}