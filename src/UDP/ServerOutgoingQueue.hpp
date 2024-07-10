#pragma once
#include "shorteners.hpp"
#include "../TS/queue.hpp"
#include "Buffer/BufferManager.hpp"
#include "../serverbase.hpp"

namespace JNet {
    namespace udp {
        template<class TPacketWrapper>
        class ServerOutgoingQueue : virtual public ServerBase<TPacketWrapper> {
            public:
                using UDPTYPES;
            public:
                ServerOutgoingQueue();
                void udpSenderClose();
                void sendPacket(ReuseablePacket packet);
        
            private:
                void handleSentPacket(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize);
        };

        template <class TPacketWrapper>
        inline ServerOutgoingQueue<TPacketWrapper>::ServerOutgoingQueue() : ServerBase<TPacketWrapper>('\0') {

        }

        template <class TPacketWrapper>
        inline void ServerOutgoingQueue<TPacketWrapper>::udpSenderClose() {
            
        }

        template <class TPacketWrapper>
        inline void ServerOutgoingQueue<TPacketWrapper>::sendPacket(ReuseablePacket packet) {
            ReuseableBuffer* sendBuffer = packet.buffer;
            auto callBack = boost::bind(
                &ServerOutgoingQueue<TPacketWrapper>::handleSentPacket
                ,this
                ,sendBuffer
                ,boost::asio::placeholders::error()
                ,boost::asio::placeholders::bytes_transferred()
            );
            this->udpSocket.async_send_to(boost::asio::buffer(sendBuffer->buffer),sendBuffer->endpoint,callBack);
        }


        template <class TPacketWrapper>
        inline void ServerOutgoingQueue<TPacketWrapper>::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize) {
            if (e.failed()) {
                std::stringstream ss;
                ss << "Error when responding:\n" << e.message() << "\n";
                std::cerr << ss.str();     
            }
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Successfully responded!\n";
            this->bufferManager.recycleBuffer(recycleableBuffer);
        }
    }
}