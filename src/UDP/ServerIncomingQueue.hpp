#pragma once
#include "shorteners.hpp"
#include "../TS/queue.hpp"
#include "../ClientServerBase/IO_Base.hpp"

namespace JNet {
    namespace udp {
        template<class TPacketWrapper>
        class ServerIncomingQueue : virtual public IO_BASE<true> {
        public:
            using UDPTYPES;
        public:
            ServerIncomingQueue();
            
            
            bool hasAvailablePacket();
            ReuseablePacket receiveIncomingPacket();
            void returnPacket(ReuseablePacket packet);
            

        protected:
            void udpReceiverClose();
            void runUdpReceive();
        private:
            void receivePackets();
            void receivePacket();

            void handlePacketReceive(ReuseableBuffer* recycleableBuffer ,const boost::system::error_code& e, size_t messageSize);
            



        private:
            JNet::ts::Queue<ReuseableBuffer*> incomingPackets;
        };

        template <class TPacketWrapper>
        inline ServerIncomingQueue<TPacketWrapper>::ServerIncomingQueue() : IO_BASE<true>('\0') {

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
            this->bufferManager.recycleBuffer(packet.buffer);
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::udpReceiverClose() {
            incomingPackets.clear();
        
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
            ReuseableBuffer* buffer = this->bufferManager.getBuffer();
            auto callback = boost::bind(&ServerIncomingQueue<TPacketWrapper>::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred);
            
            this->udpSocket.async_receive_from(
                boost::asio::buffer(buffer->buffer), 
                buffer->endpoint
                , callback
            );
            std::cout << "Waiting for async receive\n";
        }

        template <class TPacketWrapper>
        inline void ServerIncomingQueue<TPacketWrapper>::handlePacketReceive(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, size_t messageSize) {

            if (this->shouldClose) {
                if (debugFlagActive<DebugFlag::serverDebug>()) 
                    std::cout << "Unexecuted receive due to server closing\n";
                this->bufferManager.recycleBuffer(recycleableBuffer);
                return;
            }
            receivePacket();
            if (e.failed()) {
                std::stringstream ss;
                ss << "Error when handling packet Receive:\n" << e.message() << "\n";
                std::cerr << ss.str();
                this->bufferManager.recycleBuffer(recycleableBuffer);
                return;
            }
            
            
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Handling received packet" << "\n";

            this->incomingPackets.push(recycleableBuffer);
        }
    }
}