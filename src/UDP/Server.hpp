#pragma once
#include "../defines.hpp"
#include "../TS/queue.hpp"
#include "../Messages/message.hpp"
#include "../Context.hpp"
#include "packet.hpp"


namespace JNet {
    namespace udp {
        

        class Server {
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
            void handleReceive(const boost::system::error_code& e, size_t messageSize);
            JNet::ts::Queue<Packet<>> messages;
            std::mutex messagesMutex;
            bool shouldClose = false;
            uint64_t messageCount = 0;
            boost::asio::ip::udp::socket socket;
            boost::asio::ip::udp::endpoint remoteEndpoint;
            std::array<uint8_t, UINT16_MAX + 1> receiveBuffer;

        };
    }
    
}