#pragma once
//#include "shorteners.hpp"
#include "../TS/queue.hpp"
#include "Buffer/BufferManager.hpp"
#include "../serverbase.hpp"
#include "ReuseablePacket.hpp"

namespace JNet {
    namespace udp {
        template<class TPacketWrapper>
        class Sender : virtual public ServerBase<TPacketWrapper> {
            public:
                //using UDPTYPES;
                using ReuseableBuffer = udp::ReuseableBuffer<udp::bufferSize,true>;
                using BufferManager = udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, true>;
                using ReuseablePacket = udp::ReuseablePacket<TPacketWrapper, udp::bufferSize, true>;
            public:
                Sender();
                void udpSenderClose();
                void sendPacket(ReuseablePacket packet);
                /** gets an empty packet which can be send using this instance
                */        
                ReuseablePacket getEmptyPacket();
            private:
                void handleSentPacket(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize);
        };

        template <class TPacketWrapper>
        inline Sender<TPacketWrapper>::Sender() : ServerBase<TPacketWrapper>('\0') {

        }

        template <class TPacketWrapper>
        inline void Sender<TPacketWrapper>::udpSenderClose() {
            
        }

        template <class TPacketWrapper>
        inline void Sender<TPacketWrapper>::sendPacket(ReuseablePacket packet) {
            ReuseableBuffer* sendBuffer = packet.buffer;
            auto callBack = boost::bind(
                &Sender<TPacketWrapper>::handleSentPacket
                ,this
                ,sendBuffer
                ,boost::asio::placeholders::error()
                ,boost::asio::placeholders::bytes_transferred()
            );
            this->udpSocket.async_send_to(boost::asio::buffer(sendBuffer->buffer),sendBuffer->endpoint,callBack);
        }

        template <class TPacketWrapper>
        inline typename Sender<TPacketWrapper>::ReuseablePacket Sender<TPacketWrapper>::getEmptyPacket() {
            return ReuseablePacket(this->bufferManager.getBuffer());
        }

        template <class TPacketWrapper>
        inline void Sender<TPacketWrapper>::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize)
        {
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