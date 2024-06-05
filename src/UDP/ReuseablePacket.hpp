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
        template <const uint32_t reuseableBufferSize = bufferSize, bool includeEndpoint = false>
        class ReuseablePacket {
            friend class JNet::Client;
        public:
            ReuseablePacket() = delete;
            ReuseablePacket(const ReuseablePacket<reuseableBufferSize, includeEndpoint>& other) = delete;
            ReuseablePacket(const ReuseablePacket<reuseableBufferSize, includeEndpoint>&& other);
            ReuseablePacket<>& operator= (const ReuseablePacket<reuseableBufferSize, includeEndpoint>& other) = delete;
        public:
            Packet<reuseableBufferSize>& packet();
        private:
            ReuseablePacket(ReuseableBuffer<reuseableBufferSize, includeEndpoint>* buffer);
        private:
            ReuseableBuffer<reuseableBufferSize>* buffer;
        };


        template <uint32_t reuseableBufferSize, bool includeEndpoint>
        Packet<reuseableBufferSize>& ReuseablePacket<reuseableBufferSize, includeEndpoint>::packet() {
            return *(Packet<>*)buffer;
        }

        template <uint32_t reuseableBufferSize, bool includeEndpoint>
        ReuseablePacket<reuseableBufferSize, includeEndpoint>::ReuseablePacket(ReuseableBuffer<reuseableBufferSize, includeEndpoint>* buffer) {
            this->buffer = buffer;
        }

        template <uint32_t reuseableBufferSize, bool includeEndpoint>
        ReuseablePacket<reuseableBufferSize, includeEndpoint>::ReuseablePacket(const ReuseablePacket<reuseableBufferSize, includeEndpoint>&& other) {
            buffer = other.buffer;
        }
    }
}

    