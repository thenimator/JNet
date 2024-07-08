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
                void sendNextPacket();

            private:
                JNet::ts::Queue<ReuseableBuffer*> outgoingPackets;
                boost::asio::thread_pool udpSender{2};
        };

        template <class TPacketWrapper>
        inline ServerOutgoingQueue<TPacketWrapper>::ServerOutgoingQueue() : ServerBase<TPacketWrapper>('\0') {

        }

        template <class TPacketWrapper>
        inline void ServerOutgoingQueue<TPacketWrapper>::udpSenderClose() {
            outgoingPackets.clear(); 
            udpSender.join(); 
        }

        template <class TPacketWrapper>
        inline void ServerOutgoingQueue<TPacketWrapper>::sendPacket(ReuseablePacket packet) {
            outgoingPackets.push(packet.buffer);

            boost::asio::post(udpSender, boost::bind(&ServerOutgoingQueue<TPacketWrapper>::sendNextPacket,this));
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

        template <class TPacketWrapper>
        inline void ServerOutgoingQueue<TPacketWrapper>::sendNextPacket() {
            ReuseableBuffer* sendBuffer = outgoingPackets.consumeFront();
            auto callBack = boost::bind(
                &ServerOutgoingQueue<TPacketWrapper>::handleSentPacket
                ,this
                ,sendBuffer
                ,boost::asio::placeholders::error()
                ,boost::asio::placeholders::bytes_transferred()
            );
            this->udpSocket.async_send_to(boost::asio::buffer(sendBuffer->buffer),sendBuffer->endpoint,callBack);
        }
    }
}