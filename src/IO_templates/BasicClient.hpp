#pragma once
#include "../TS/queue.hpp"
#include "../Messages/communication.hpp"
#include "../defines.hpp"
#include "../TS/ts.hpp"
#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/asio/placeholders.hpp>
#include <type_traits>
#include "../Context.hpp"
#include "../UDP/ReuseablePacket.hpp"


namespace JNet {
    //needs to be refactored to support all protocols
    template<typename InternetProtocol>
    class BasicClient {
    public:
        typedef boost::asio::ip::basic_endpoint<InternetProtocol> Endpoint;
        typedef boost::asio::basic_datagram_socket<InternetProtocol> Socket;
        typedef boost::asio::ip::basic_resolver<InternetProtocol> Resolver;
        typedef udp::ReuseableBuffer<> ReuseableBuffer;
        
    public:
        BasicClient(Context& givenContext) : context(givenContext), socket(givenContext.getAsioContext()) {

        }
        BasicClient(const BasicClient&) = delete;
        BasicClient& operator=(BasicClient&) = delete;
        ~BasicClient() {
            sender.join();
        }

        udp::ReuseablePacket<> getPacket() {
            return udp::ReuseablePacket<>(bufferManager.getBuffer());
        }

        void sendPacket(udp::ReuseablePacket<> packet) {
            if (host == "") {
                if (debugFlagActive<DebugFlag::clientDebug>()) 
                    std::cout << "No host given but packet send attempted\n";
                bufferManager.recycleBuffer(packet.buffer);

                return;
            }
            outgoing.push(packet.buffer);

            boost::asio::post(sender, boost::bind(&BasicClient<InternetProtocol>::sendNextMessageToHost,this));
        }
        
        //out of work
        // DO NOT USE
        void sendPacket(udp::Packet<>& message) {
            if (host == "")
                return;
            udp::ReuseableBuffer<>* sendBuffer = bufferManager.getBuffer();
            //message.writeToBuffer(&sendBuffer, sendBuffer->buffer.size());
            outgoing.push(sendBuffer);

            boost::asio::post(sender, boost::bind(&BasicClient<InternetProtocol>::sendNextMessageToHost,this));
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
        ts::BufferManager<> bufferManager;

    private:
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
        /*void sendMessagesToHost() {
            if (debugFlagActive<DebugFlag::clientDebug>()) 
                std::cout << "Sending all messages\n";
            while (!outgoing.empty()) {
                sendNextMessageToHost();
            }
            if (debugFlagActive<DebugFlag::clientDebug>()) 
                std::cout << "Sent all messages\n";
            
        }*/
        void handleSentMessage(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize) {
            
            bufferManager.recycleBuffer(recycleableBuffer);
            if (e.failed()) {
                std::cerr << "WHY YOU " << e.value() << "\n";
            }
        }

        void handleSentMessage(ReuseableBuffer* recycleableBuffer) {
            std::cerr << "WHY CAN'T I CALL THE OTHER\n";
            bufferManager.recycleBuffer(recycleableBuffer);
        }

        void sendNextMessageToHost() {
            
            ReuseableBuffer* sendBuffer = outgoing.consumeFront();
            auto callBack = boost::bind(
                &BasicClient<InternetProtocol>::handleSentMessage
                ,this
                ,sendBuffer
                ,boost::asio::placeholders::error()
                ,boost::asio::placeholders::bytes_transferred()
            );
            //socket.async_send(boost::asio::buffer(sendBuffer->buffer) , boost::bind(&BasicClient::handleSentMessage, this, sendBuffer, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
            //socket.async_send(boost::asio::buffer(sendBuffer->buffer), boost::bind(&BasicClient<InternetProtocol>::handleSentMessage, this, sendBuffer, boost::asio::placeholders::error(), boost::asio::placeholders::bytes_transferred()));
            socket.async_send(boost::asio::buffer(sendBuffer->buffer)
            ,callBack
            //,boost::bind(
            //&BasicClient<InternetProtocol>::handleSentMessage
            //
            //,sendBuffer
            //,boost::asio::placeholders::error()
            //,boost::asio::placeholders::bytes_transferred()
            //)
            );
            if (debugFlagActive<DebugFlag::clientDebug>()) {
                std::stringstream ss;

                ss << "Sent to: " << socket.remote_endpoint() << "\n";
            
                std::cout << ss.str();
            }
        }    

        
    };
}