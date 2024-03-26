#pragma once
#include "defines.hpp"

namespace JNet {
    namespace udp {



        class Client {
        public:
            Client(std::string_view host);
            void connect();
            void disconnect();
            void receiveData();
            bool hasConnection();
            ~Client();
        private:
            bool shouldDisconnect = false;
            bool activeConnection = false;
            boost::asio::io_context context;
            std::thread receiver;
            std::queue<Message> messages;
            boost::asio::ip::udp::endpoint endpoint;
            uint64_t messageCount = 0;

        };




    }
}