#pragma once
#include "../Context.hpp"

namespace JNet {
    namespace tcp {
        using Socket = boost::asio::ip::tcp::socket;
        /** @brief Represents a call-response tcp connection 
         * @attention is automatically deleted by itself once the response has been sent to the caller
         * @warning You're only allowed to send one message per call-response connection as the pointer to it is invalidated aber calling sendMessage()
        */
        class Connection {
        public:
            Connection(Connection&) = delete;
            Connection(const Connection&) = delete;
            Socket& getSocket();
            static Connection* create(JNet::Context &context);
            void sendMessage(const std::string_view message);
            template<class T>
            void sendMessage(const std::vector<T>& message);
            template<class T, size_t size>
            void sendMessage(const std::array<T, size>& message);
            void sendMessage(const void* buffer, size_t bytes);
        private:
            
            void handleMessageSent(const boost::system::error_code& e);
            Connection(JNet::Context& context);
        private:
            Socket socket;
        };

        inline void Connection::sendMessage(const void* buffer, size_t bytes) {
            auto callback = boost::bind(&Connection::handleMessageSent, this, boost::asio::placeholders::error());
            boost::asio::write(socket,boost::asio::buffer(buffer,bytes));
        }

        void Connection::handleMessageSent(const boost::system::error_code &e)
        {

            if (e.failed()) {
                std::cerr << "Message responding failed\n";
            }


            delete this;
        }

        Connection::Connection(JNet::Context &context) : socket(context.getAsioContext()) {
        }

        Socket& Connection::getSocket() {
            return socket;
        }

        inline Connection *Connection::create(JNet::Context& context) {
            return new Connection(context);
        }
        inline void Connection::sendMessage(const std::string_view message) {
            sendMessage(message.data(), message.size());
        }

        template <class T>
        inline void Connection::sendMessage(const std::vector<T>& message) {
            sendMessage(message.data(), message.size() * sizeof(T));
        }

        template <class T, size_t size>
        inline void Connection::sendMessage(const std::array<T, size>& message) {
            sendMessage(message.data(), message.size()*sizeof(T));
        }
    }
}