#pragma once
//#include "shorteners.hpp"
#include "../TS/queue.hpp"
#include "Buffer/BufferManager.hpp"
#include "../serverbase.hpp"
#include "ReuseablePacket.hpp"

namespace JNet {
    namespace udp {
        template<class TPacketWrapper, bool TIncludeEndpoint = true>
        class SenderBase : virtual public IO_BASE<true> {
            public:
                //using UDPTYPES;
                using ReuseableBuffer = udp::ReuseableBuffer<udp::bufferSize,true>;
                using BufferManager = udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, true>;
                using ReuseablePacket = udp::ReuseablePacket<TPacketWrapper, udp::bufferSize, true>;
            public:
                SenderBase();
                void udpSenderClose();
                /** gets an empty packet which can be send using this instance
                */        
                ReuseablePacket getEmptyPacket();
            protected:
                void handleSentPacket(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, std::size_t messageSize);
        };








        template<class TPacketWrapper, bool TIncludeEndpoint = true>
        class Sender {

        };

        template<class TPacketWrapper>
        class Sender<TPacketWrapper, false> : virtual public SenderBase<TPacketWrapper, false> {
        public:
            using ReuseableBuffer = udp::ReuseableBuffer<udp::bufferSize,true>;
            using BufferManager = udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, true>;
            using ReuseablePacket = udp::ReuseablePacket<TPacketWrapper, udp::bufferSize, true>;
        public:
            Sender();
            void sendPacket(ReuseablePacket packet);
        };

        template<class TPacketWrapper>
        class Sender<TPacketWrapper, true> : virtual public SenderBase<TPacketWrapper, true> {
        public:
            using ReuseableBuffer = udp::ReuseableBuffer<udp::bufferSize,true>;
            using BufferManager = udp::BufferManager<udp::bufferSize, SafetyFlag::threadSafe, true>;
            using ReuseablePacket = udp::ReuseablePacket<TPacketWrapper, udp::bufferSize, true>;
        public:
            Sender();
            void sendPacket(ReuseablePacket packet);
        };

        template <class TPacketWrapper, bool TIncludeEndpoint>
        inline SenderBase<TPacketWrapper, TIncludeEndpoint>::SenderBase() : IO_BASE<TIncludeEndpoint>('\0') {

        }

        template <class TPacketWrapper, bool TIncludeEndpoint>
        inline void SenderBase<TPacketWrapper, TIncludeEndpoint>::udpSenderClose() {
            
        }

        template <class TPacketWrapper, bool TIncludeEndpoint>
        inline typename SenderBase<TPacketWrapper, TIncludeEndpoint>::ReuseablePacket SenderBase<TPacketWrapper, TIncludeEndpoint>::getEmptyPacket() {
            return ReuseablePacket(this->bufferManager.getBuffer());
        }

        template <class TPacketWrapper, bool TIncludeEndpoint>
        inline void SenderBase<TPacketWrapper, TIncludeEndpoint>::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize) {
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
        inline Sender<TPacketWrapper, true>::Sender() : SenderBase<TPacketWrapper, true>(), IO_BASE<true>('\0') {

        }

        template <class TPacketWrapper>
        inline void Sender<TPacketWrapper, true>::sendPacket(ReuseablePacket packet) {

            ReuseableBuffer* sendBuffer = packet.buffer;
            auto callBack = boost::bind(
                &Sender<TPacketWrapper, true>::handleSentPacket
                ,this
                ,sendBuffer
                ,boost::asio::placeholders::error()
                ,boost::asio::placeholders::bytes_transferred()
            );
            this->udpSocket.async_send_to(boost::asio::buffer(sendBuffer->buffer),sendBuffer->endpoint,callBack);
        }


        template <class TPacketWrapper>
        inline Sender<TPacketWrapper, false>::Sender() : SenderBase<TPacketWrapper, false>(), IO_BASE<false>('\0') {

        }

        template <class TPacketWrapper>
        inline void Sender<TPacketWrapper, false>::sendPacket(ReuseablePacket packet) {

            ReuseableBuffer* sendBuffer = packet.buffer;
            auto callBack = boost::bind(
                &Sender<TPacketWrapper, false>::handleSentPacket
                ,this
                ,sendBuffer
                ,boost::asio::placeholders::error()
                ,boost::asio::placeholders::bytes_transferred()
            );
            this->udpSocket.async_send(boost::asio::buffer(sendBuffer->buffer),callBack);
        }
    }
}