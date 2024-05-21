#pragma once
#include "../TS/queue.hpp"
#include "../Messages/message.hpp"
#include "../defines.hpp"
#include "../TS/ts.hpp"
#include <boost/asio/basic_datagram_socket.hpp>
#include "../Context.hpp"


namespace JNet {
    constexpr uint32_t bufferSize = 0x10000;
    template<typename InternetProtocol>
    class BasicClient {
    public:
        typedef boost::asio::ip::basic_endpoint<InternetProtocol> Endpoint;
        typedef boost::asio::basic_datagram_socket<InternetProtocol> Socket;
        typedef boost::asio::ip::basic_resolver<InternetProtocol> Resolver;
        
    public:
        BasicClient(Context& givenContext) : context(givenContext), socket(givenContext.getAsioContext()) {

        }
        BasicClient(const BasicClient&) = delete;
        BasicClient& operator=(BasicClient&) = delete;
        void sendMessage(std::unique_ptr<Message> message) {
            if (host == "")
                return;

            outgoing.push(std::move(message));

            if (!sender.joinable())
                return;
            sender.join();
            sender = std::thread(boost::bind(&BasicClient::sendNextMessageToHost, this));
        }
        void resolveHost() {
            Resolver resolver(context.getAsioContext());
            try {

                remoteEndpoint = *resolver.resolve(host,"16632").begin();
                
            } catch (boost::system::system_error& e) {
                std::cout << "Error when connecting with " << host << std::endl;
                std::cerr << e.what() << std::endl;
                std::cerr << "Code: " << e.code() << std::endl;
            }
        }
        void connect(const std::string& host) {
            this->host = host;
            resolveHost();
            try {
                socket.connect(remoteEndpoint);
            } catch (boost::system::system_error& e) {
                std::cerr << e.what() << std::endl;
            }

        }
        std::unique_ptr<Message> receiveMessage() {
            return incoming.consumeFront();
        }
    protected:
        Context& context;
        Socket socket;

        std::string host = "";
        Endpoint remoteEndpoint;
        std::thread receiver;
        std::thread sender;
        uint64_t messageCount = 0;
        ts::Queue<std::unique_ptr<Message>> incoming;
        ts::Queue<std::unique_ptr<Message>> outgoing;
        std::array<uint8_t, bufferSize> receiveBuffer;
        ts::BufferManager<bufferSize> bufferManager;

    private:
        void sendMessagesToHost() {
            while (!outgoing.empty()) {
                sendNextMessageToHost();
            }
            
        }
        void sendNextMessageToHost() {
            data::ReuseableBuffer<bufferSize>* sendBuffer = bufferManager.getBuffer();
            outgoing.front()->writeToBuffer(&sendBuffer, sendBuffer->buffer.size());
            socket.async_send_to(boost::asio::buffer(sendBuffer->buffer),remoteEndpoint , boost::bind(&BasicClient::handleSentMessage, this, sendBuffer, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void handleSentMessage(data::ReuseableBuffer<bufferSize>* recycleableBuffer, boost::system::error_code& e, size_t messageSize) {
            bufferManager.recycleBuffer(recycleableBuffer);
        }
    };
}