#pragma once
#include "shorteners.hpp"
#include "../TS/queue.hpp"
#include "Buffer/BufferManager.hpp"
#include "../serverbase.hpp"

namespace JNet {
    namespace udp {
        template<class TPacketWrapper>
        class ServerIncomingQueue : public virtual ServerBase {
        public:
            using UDPTYPES;
        public:
            ServerIncomingQueue(uint16_t port);
            /** gets and empty packet which can be send using this server
            */        
            ReuseablePacket getEmptyPacket();
            void sendPacket(ReuseablePacket packet);
            bool hasAvailablePacket();
            ReuseablePacket receiveIncomingPacket();
            void returnPacket(ReuseablePacket packet);
            

        protected:
            void udpReceiverClose();
            void runUdpReceive();

            void receivePackets();
            void receivePacket();

            void handlePacketReceive(ReuseableBuffer* recycleableBuffer ,const boost::system::error_code& e, size_t messageSize);
            void handleSentPacket(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize);
            void sendNextPacket();



        protected:
            BufferManager bufferManager;
            udp::Socket udpSocket;
            udp::Endpoint udpEndpoint;
            JNet::ts::Queue<ReuseableBuffer*> incomingPackets;
            JNet::ts::Queue<ReuseableBuffer*> outgoingPackets;
            boost::asio::thread_pool udpSender{2};
        };

        template <class TPacketWrapper>
        inline ServerIncomingQueue<TPacketWrapper>::ServerIncomingQueue(uint16_t port) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {

        }

        template <class TPacketWrapper>
        inline typename ServerIncomingQueue<TPacketWrapper>::ReuseablePacket ServerIncomingQueue<TPacketWrapper>::getEmptyPacket() {
            return ReuseablePacket(bufferManager.getBuffer());
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::sendPacket(ReuseablePacket packet) {
            outgoingPackets.push(packet.buffer);

            boost::asio::post(udpSender, boost::bind(&ServerIncomingQueue<TPacketWrapper>::sendNextPacket,this));
        }

        template <class TPacketWrapper>
        inline bool ServerIncomingQueue<TPacketWrapper>::hasAvailablePacket() {
            return !incomingPackets.empty();
        }

        template <class TPacketWrapper>
        inline typename ServerIncomingQueue<TPacketWrapper>::ReuseablePacket ServerIncomingQueue<TPacketWrapper>::receiveIncomingPacket() {
            return ReuseablePacket(incomingPackets.consumeFront());
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::returnPacket(ReuseablePacket packet) {
            bufferManager.recycleBuffer(packet.buffer);
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::udpReceiverClose() {
            outgoingPackets.clear();
            incomingPackets.clear();
            udpSender.join();
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::runUdpReceive() {
            receivePackets();
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::receivePackets() {
            receivePacket();
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::receivePacket() {
            ReuseableBuffer* buffer = bufferManager.getBuffer();
            auto callback = boost::bind(&ServerIncomingQueue<TPacketWrapper>::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred);
            
            udpSocket.async_receive_from(
                boost::asio::buffer(buffer->buffer), 
                buffer->endpoint
                , callback
            );
            std::cout << "Waiting for async receive\n";
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::handlePacketReceive(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, size_t messageSize) {
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

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize) {
            if (e.failed()) {
                std::stringstream ss;
                ss << "Error when responding:\n" << e.message() << "\n";
                std::cerr << ss.str();     
            }
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Successfully responded!\n";
            bufferManager.recycleBuffer(recycleableBuffer);
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::sendNextPacket() {
            ReuseableBuffer* sendBuffer = outgoingPackets.consumeFront();
            auto callBack = boost::bind(
                &ServerIncomingQueue<TPacketWrapper>::handleSentPacket
                ,this
                ,sendBuffer
                ,boost::asio::placeholders::error()
                ,boost::asio::placeholders::bytes_transferred()
            );
            udpSocket.async_send_to(boost::asio::buffer(sendBuffer->buffer),sendBuffer->endpoint,callBack);
        }
    }
}