#pragma once
#include "defines.hpp"

namespace JNet {
    namespace udp {
        class Server {
        public:
            Server();
            void close();
            ~Server();
        private:
            void run();
            bool shouldClose = false;
            boost::asio::io_context context;
            std::thread sender;
            std::queue<Message> messages;
            uint64_t messageCount = 0;
            boost::asio::ip::udp::socket socket;

        };
    }
    
}

