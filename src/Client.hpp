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
    //TODO: Client and Server inherit from shared base class
    //TODO: Turn client and Server into template classes to allow for customization of udp buffersize and custom wrapper
    //TODO: Context always has to call
    class Client {
    public:
        typedef udp::ReuseableBuffer<udp::bufferSize,false> ReuseableBuffer;
        typedef udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, false> BufferManager;
        typedef udp::ReuseablePacket<udp::bufferSize, false> ReuseablePacket;
        typedef udp::Packet<udp::bufferSize> Packet;
        
    public:
        //requires its own context
        Client();
        Client(const Client&) = delete;
        Client& operator=(Client&) = delete;
        ~Client();
        ReuseablePacket getPacket();
        void sendPacket(ReuseablePacket packet);
        void connect(const std::string& host);
        /** @brief closes the currently active connection to a server
         * @brief guaranteed to wait at least finishDuration for outstanding operations
         */
        void disconnect(std::chrono::microseconds finishDuration = std::chrono::microseconds(100));
        bool hasAvailablePacket();
        ReuseablePacket receiveIncomingPacket();
        void returnPacket(ReuseablePacket packet);
        bool hasConnection();
        //std::unique_ptr<udp::Packet<>> receiveMessage();
    private:
        void receive();
        void handlePacketReceive(ReuseableBuffer* recycleableBuffer ,const boost::system::error_code& e, size_t messageSize);
    private:
        bool shouldDisconnect = true;
        Context context;
        std::string host = "";
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

    
}