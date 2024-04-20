#pragma once
#include "../defines.hpp"
#include "../TS/queue.hpp"


namespace JNet {
    namespace udp {

        union MessageBuffer
        {
            Message message;
            std::array<char,sizeof(Message)> buffer;
        };
        

        class Server {
        public:
            Server(boost::asio::io_context& context);
            void close();
            ~Server();
        private:
            void run();
            void receive();
            void respond(const boost::system::error_code& e, size_t messageSize);
            void handleMessage(const boost::system::error_code& e, size_t messageSize);
            void handleReceive(const boost::system::error_code& e, size_t messageSize);
            JNet::ts::Queue<Message> messages;
            std::mutex messagesMutex;
            bool shouldClose = false;
            uint64_t messageCount = 0;
            boost::asio::ip::udp::socket socket;
            MessageBuffer receiveBuffer;
            boost::asio::ip::udp::endpoint remoteEndpoint;

        };
    }
    
}