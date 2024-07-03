#pragma once
#include "shorteners.hpp"
#include "../TS/queue.hpp"
#include "Buffer/BufferManager.hpp"
#include "../serverbase.hpp"
#include "ServerQueueCallbackHelper.hpp"

namespace JNet {
    namespace udp {
        template<class TPacketWrapper>
        class Serverqueue : public virtual ServerBase {
        public:
            using UDPTYPES;
        public:
            Serverqueue(uint16_t port);
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
            


        protected:
            ServerQueueCallBackHelper<TPacketWrapper> helper;
        };

        template <class TPacketWrapper>
        inline Serverqueue<TPacketWrapper>::Serverqueue(uint16_t port) : helper(context.getAsioContext(), port, shouldClose) {

        }

        template <class TPacketWrapper>
        inline typename Serverqueue<TPacketWrapper>::ReuseablePacket Serverqueue<TPacketWrapper>::getEmptyPacket() {
            return ReuseablePacket(helper.bufferManager.getBuffer());
        }

        template <class TPacketWrapper>
        inline void Serverqueue<TPacketWrapper>::sendPacket(ReuseablePacket packet) {
            helper.outgoingPackets.push(packet.buffer);

            boost::asio::post(helper.udpSender, boost::bind(&ServerQueueCallBackHelper<TPacketWrapper>::sendNextPacket,&helper));
        }

        template <class TPacketWrapper>
        inline bool Serverqueue<TPacketWrapper>::hasAvailablePacket() {
            return !helper.incomingPackets.empty();
        }

        template <class TPacketWrapper>
        inline typename Serverqueue<TPacketWrapper>::ReuseablePacket Serverqueue<TPacketWrapper>::receiveIncomingPacket() {
            return ReuseablePacket(helper.incomingPackets.consumeFront());
        }

        template <class TPacketWrapper>
        inline void Serverqueue<TPacketWrapper>::returnPacket(ReuseablePacket packet) {
            helper.bufferManager.recycleBuffer(packet.buffer);
        }

        template <class TPacketWrapper>
        inline void Serverqueue<TPacketWrapper>::udpReceiverClose() {
            helper.outgoingPackets.clear();
            helper.incomingPackets.clear();
            helper.udpSender.join();
        }

        template <class TPacketWrapper>
        inline void Serverqueue<TPacketWrapper>::runUdpReceive() {
            receivePackets();
        }

        template <class TPacketWrapper>
        inline void Serverqueue<TPacketWrapper>::receivePackets() {
            helper.receivePacket();
        }

    }
}