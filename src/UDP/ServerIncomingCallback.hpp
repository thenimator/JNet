#pragma once
#include "../ClientServerBase/IO_Base.hpp"

namespace JNet {
    namespace udp {
        template<class TPacketWrapper>
        class ServerIncomingCallback : public virtual IO_BASE<true> {
        public:
            using ReuseableBuffer = udp::ReuseableBuffer<JNet::udp::bufferSize,true>; 
            using BufferManager = udp::BufferManager<JNet::udp::bufferSize, SafetyFlag::threadSafe, true>; 
            using ReuseablePacket = JNet::udp::ReuseablePacket<TPacketWrapper ,JNet::udp::bufferSize, true>;
;
        public:
            ServerIncomingCallback();
            void setCallback(std::function<void(ReuseablePacket)> callback);
        protected:
            void udpReceiverClose();
            void runUdpReceive();
        private:
            void receivePackets();
            void receivePacket();

            void handlePacketReceive(ReuseableBuffer* recycleableBuffer ,const boost::system::error_code& e, size_t messageSize);
        private:
            std::function<void(ReuseablePacket)> callback = nullptr;
        };

        template <class TPacketWrapper>
        inline ServerIncomingCallback<TPacketWrapper>::ServerIncomingCallback() : IO_BASE<true>('\0') {

        }

        template <class TPacketWrapper>
        inline void ServerIncomingCallback<TPacketWrapper>::setCallback(std::function<void(ReuseablePacket)> callback) {
            if (this->callback != nullptr) {
                throw std::runtime_error("Callback can't be changed after initializing it");
            }
            if (callback == nullptr) {
                throw std::runtime_error("Given callback is nullptr");
            }
            this->callback = callback;
        }

        template <class TPacketWrapper>
        inline void ServerIncomingCallback<TPacketWrapper>::udpReceiverClose() {
        }

        template <class TPacketWrapper>
        inline void ServerIncomingCallback<TPacketWrapper>::runUdpReceive() {
            if (callback == nullptr)
                throw std::runtime_error("You need to set a callback before running the server!");
            receivePackets();
        }

        template <class TPacketWrapper>
        inline void ServerIncomingCallback<TPacketWrapper>::receivePackets() {
            receivePacket();
        }
        template <class TPacketWrapper>
        inline void ServerIncomingCallback<TPacketWrapper>::receivePacket() {
            ReuseableBuffer* buffer = this->bufferManager.getBuffer();
            auto callback = boost::bind(&ServerIncomingCallback<TPacketWrapper>::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred);
            
            this->udpSocket.async_receive_from(
                boost::asio::buffer(buffer->buffer), 
                buffer->endpoint
                , callback
            );
            std::cout << "Waiting for async receive\n";
        }

        template <class TPacketWrapper>
        inline void ServerIncomingCallback<TPacketWrapper>::handlePacketReceive(ReuseableBuffer* recycleableBuffer, const boost::system::error_code& e, size_t messageSize) {

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
            this->callback(ReuseablePacket(recycleableBuffer));
        }
    }
}