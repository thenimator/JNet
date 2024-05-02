#pragma once
#include "../defines.hpp"
#include "../Context.hpp"
#include "../TS/queue.hpp"
#include "../message.hpp"
#include "../Interfaces/IO_ClientInterface.hpp"


namespace JNet {
    namespace udp {
        class Client : IO_ClientInterface {
        public:
            Client(Context& context);
            void connect(std::string_view host);
            void disconnect();
            void receiveData();
            bool hasConnection();
            ~Client();
        private:
            bool shouldDisconnect = false;
            bool activeConnection = false;
            Context& context;
            std::thread receiver;
            JNet::ts::Queue<Message> messages;
            boost::asio::ip::udp::endpoint endpoint;
            uint64_t messageCount = 0;

        };




    }
}