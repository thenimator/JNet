#pragma once
#include "udp.hpp"
#include "../defines.hpp"
#include "Buffer/buffer.hpp"
#include "packet.hpp"
#include "../ClientDeclaration.hpp"

namespace JNet {
    namespace udp {


        /** @brief Wrapper of a ReuseableBuffer to be used as Packet
        * @attention Needs to be returned to the client it was created by
        */
        template <const uint32_t reuseableBufferSize = bufferSize>
        class ReuseablePacket {
            template<typename InternetProtocol>
            friend class JNet::BasicClient;
        public:
            ReuseablePacket() = delete;
            ReuseablePacket(const ReuseablePacket<reuseableBufferSize>& other) = delete;
            ReuseablePacket(const ReuseablePacket<reuseableBufferSize>&& other);
            ReuseablePacket<>& operator= (const ReuseablePacket<reuseableBufferSize>& other) = delete;
        public:
            Packet<reuseableBufferSize>& packet();
        private:
            ReuseablePacket(ReuseableBuffer<reuseableBufferSize>* buffer);
        private:
            ReuseableBuffer<reuseableBufferSize>* buffer;
        };


        template <uint32_t reuseableBufferSize>
        Packet<reuseableBufferSize>& ReuseablePacket<reuseableBufferSize>::packet() {
            return *(Packet<>*)buffer;
        }

        template <uint32_t reuseableBufferSize>
        ReuseablePacket<reuseableBufferSize>::ReuseablePacket(ReuseableBuffer<reuseableBufferSize>* buffer) {
            this->buffer = buffer;
        }

        template <uint32_t reuseableBufferSize>
        ReuseablePacket<reuseableBufferSize>::ReuseablePacket(const ReuseablePacket<reuseableBufferSize>&& other) {
            buffer = other.buffer;
        }
    }
}

    