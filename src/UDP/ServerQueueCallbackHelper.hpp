#pragma once
#include "shorteners.hpp"
#include "../TS/queue.hpp"
#include "Buffer/BufferManager.hpp"
#include "../serverbase.hpp"


namespace JNet {
    namespace udp {
        template<class TPacketWrapper>
        class ServerQueueCallBackHelper {
        public:
            using UDPTYPES;
            //using BufferManager = BufferManager<bufferSize, SafetyFlag::threadSafe, true>;
            //using ReuseableBuffer = ReuseableBuffer<bufferSize, true>;

        public:
            ServerQueueCallBackHelper(boost::asio::io_context& context, uint16_t port, bool& givenShouldClose) : udpSocket(context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)), shouldClose{givenShouldClose} {};

            void receivePacket();

            void handlePacketReceive(ReuseableBuffer* recycleableBuffer ,const boost::system::error_code& e, size_t messageSize);
            void handleSentPacket(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize);
            void sendNextPacket();


        public:
            BufferManager bufferManager;
            udp::Socket udpSocket;
            udp::Endpoint udpEndpoint;
            JNet::ts::Queue<ReuseableBuffer*> incomingPackets;
            JNet::ts::Queue<ReuseableBuffer*> outgoingPackets;
            boost::asio::thread_pool udpSender{2};
        private:
            bool& shouldClose;
        };

        template<class TPacketWrapper>
        inline void ServerQueueCallBackHelper<TPacketWrapper>::receivePacket() {
            ReuseableBuffer* buffer = bufferManager.getBuffer();
            auto callback = boost::bind(&ServerQueueCallBackHelper<TPacketWrapper>::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred);
            
            udpSocket.async_receive_from(
                boost::asio::buffer(buffer->buffer), 
                buffer->endpoint
                , callback
            );
            std::cout << "Waiting for async receive\n";
        }

        template<class TPacketWrapper>
        inline void ServerQueueCallBackHelper<TPacketWrapper>::handlePacketReceive(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, size_t messageSize) {
            std::cout << "handlePacketReceive called\n";

            if (shouldClose) {
                if (debugFlagActive<DebugFlag::serverDebug>()) 
                    std::cout << "Unexecuted receive due to server closing\n";
                bufferManager.recycleBuffer(recycleableBuffer);
                return;
            }
            receivePacket();
            if (e.failed()) {
                std::stringstream ss;
                ss << "Error when handling packet Receive:\n" << e.message() << "\n";
                std::cerr << ss.str();
                bufferManager.recycleBuffer(recycleableBuffer);
                return;
            }
            
            
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Handling received packet" << "\n";

            incomingPackets.push(recycleableBuffer);
        }

        template<class TPacketWrapper>
        inline void ServerQueueCallBackHelper<TPacketWrapper>::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize) {
            if (e.failed()) {
                std::stringstream ss;
                ss << "Error when responding:\n" << e.message() << "\n";
                std::cerr << ss.str();     
            }
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Successfully responded!\n";
            bufferManager.recycleBuffer(recycleableBuffer);
        }

        template<class TPacketWrapper>
        inline void ServerQueueCallBackHelper<TPacketWrapper>::sendNextPacket() {
            ReuseableBuffer* sendBuffer = outgoingPackets.consumeFront();
            auto callBack = boost::bind(
                &ServerQueueCallBackHelper<TPacketWrapper>::handleSentPacket
                ,this
                ,sendBuffer
                ,boost::asio::placeholders::error()
                ,boost::asio::placeholders::bytes_transferred()
            );
            udpSocket.async_send_to(boost::asio::buffer(sendBuffer->buffer),sendBuffer->endpoint,callBack);
        }
    }
}