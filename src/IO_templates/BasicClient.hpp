#pragma once
#include "../TS/queue.hpp"
#include "../Messages/communication.hpp"
#include "../defines.hpp"
#include "../TS/ts.hpp"
#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/asio/placeholders.hpp>
#include <type_traits>
#include "../Context.hpp"


namespace JNet {
    constexpr uint32_t bufferSize = 0x10000;
    template<typename InternetProtocol>
    class BasicClient {
    public:
        typedef boost::asio::ip::basic_endpoint<InternetProtocol> Endpoint;
        typedef boost::asio::basic_datagram_socket<InternetProtocol> Socket;
        typedef boost::asio::ip::basic_resolver<InternetProtocol> Resolver;
        typedef data::ReuseableBuffer<bufferSize> ReuseableBuffer;
        
    public:
        BasicClient(Context& givenContext) : context(givenContext), socket(givenContext.getAsioContext()) {

        }
        BasicClient(const BasicClient&) = delete;
        BasicClient& operator=(BasicClient&) = delete;
        ~BasicClient() {
            sender.join();
        }
        
        
        void sendPacket(udp::Packet<>& message) {
            if (host == "")
                return;
            ReuseableBuffer* sendBuffer = bufferManager.getBuffer();
            //message.writeToBuffer(&sendBuffer, sendBuffer->buffer.size());
            outgoing.push(sendBuffer);

            boost::asio::post(sender, boost::bind(&BasicClient<InternetProtocol>::sendNextMessageToHost,this));
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
                if (debugFlagActive<DebugFlag::clientDebug>()) 
                    std::cout << "Conntected with: " <<
                    remoteEndpoint << "\n";
            } catch (boost::system::system_error& e) {
                std::cerr << e.what() << std::endl;
            }

        }
        std::unique_ptr<udp::Packet<>> receiveMessage() {
            return std::unique_ptr<udp::Packet<>>();
            //return incoming.consumeFront();
        }
    protected:
        Context& context;
        Socket socket;

        std::string host = "";
        Endpoint remoteEndpoint;
        std::thread receiver;
        boost::asio::thread_pool sender{1};
        
        uint64_t messageCount = 0;
        ts::Queue<std::unique_ptr<udp::Packet<>>> incoming;
        ts::Queue<ReuseableBuffer*> outgoing;
        std::array<uint8_t, bufferSize> receiveBuffer;
        ts::BufferManager<bufferSize> bufferManager;

    private:
        /*void sendMessagesToHost() {
            if (debugFlagActive<DebugFlag::clientDebug>()) 
                std::cout << "Sending all messages\n";
            while (!outgoing.empty()) {
                sendNextMessageToHost();
            }
            if (debugFlagActive<DebugFlag::clientDebug>()) 
                std::cout << "Sent all messages\n";
            
        }*/
        void handleSentMessage(ReuseableBuffer* recycleableBuffer, boost::system::error_code& e, size_t messageSize) {
            
            bufferManager.recycleBuffer(recycleableBuffer);

        }

        void handleSentMessage(ReuseableBuffer* recycleableBuffer) {
            bufferManager.recycleBuffer(recycleableBuffer);
        }

        void sendNextMessageToHost() {
            ReuseableBuffer* sendBuffer = outgoing.consumeFront();
            //socket.async_send(boost::asio::buffer(sendBuffer->buffer) , boost::bind(&BasicClient::handleSentMessage, this, sendBuffer, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
            //socket.async_send(boost::asio::buffer(sendBuffer->buffer), boost::bind(&BasicClient<InternetProtocol>::handleSentMessage, this, sendBuffer, boost::asio::placeholders::error(), boost::asio::placeholders::bytes_transferred()));
            socket.async_send(boost::asio::buffer(sendBuffer->buffer), 
            boost::bind(
            &BasicClient<InternetProtocol>::handleSentMessage
            ,this
            ,sendBuffer
            //,boost::asio::placeholders::error()
            //,boost::asio::placeholders::bytes_transferred()
            ));
            if (debugFlagActive<DebugFlag::clientDebug>()) {
                std::stringstream ss;

                ss << "Sent to: " << socket.remote_endpoint() << "\n";
            
                std::cout << ss.str();
            }
        }    

        
    };
}