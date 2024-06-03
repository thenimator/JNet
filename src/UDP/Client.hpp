#pragma once
#include "../IO_templates/BasicClient.hpp"

namespace JNet {
    namespace udp {
        typedef BasicClient<boost::asio::ip::udp> Client;
    }
}
/*#include "../defines.hpp"
#include "../Context.hpp"
#include "../TS/queue.hpp"
#include "../Messages/message.hpp"
#include "../Interfaces/IO_ClientInterface.hpp"
#include <cstdint>


namespace JNet {
    namespace udp {
        class Client : IO_ClientInterface<boost::asio::ip::tcp> {
        public:
            Client(Context& context);
            void connect(std::string_view host);
            void disconnect();
            
            bool hasConnection();
            ~Client();
        private:
            void receiveData();
            bool shouldDisconnect = false;
            bool activeConnection = false;
            
            
            

        };




    }
}*/