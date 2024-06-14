#pragma once
#include "udp.hpp"
#include "../defines.hpp"
#include "Buffer/buffer.hpp"
#include "packet.hpp"
#include "../ClientDeclaration.hpp"
#include "../ServerDeclaration.hpp"

namespace JNet {
    namespace udp {


        /** @brief Wrapper of a ReuseableBuffer to be used as Packet
        * @attention Needs to be returned to the client/server it was created by
        */
        template <const uint32_t reuseableBufferSize = bufferSize, bool includeEndpoint = false>
        class ReuseablePacket {
            friend class JNet::Client;
            friend class JNet::Server;
        public:
            ReuseablePacket() = delete;
            ReuseablePacket(const ReuseablePacket<reuseableBufferSize, includeEndpoint>& other) = delete;
            ReuseablePacket(const ReuseablePacket<reuseableBufferSize, includeEndpoint>&& other);
            ReuseablePacket<reuseableBufferSize,includeEndpoint>& operator= (const ReuseablePacket<reuseableBufferSize, includeEndpoint>& other) = delete;
        public:
            ReuseableBuffer<reuseableBufferSize, includeEndpoint>& data();
        private:
            ReuseablePacket(ReuseableBuffer<reuseableBufferSize, includeEndpoint>* buffer);
        private:
            ReuseableBuffer<reuseableBufferSize, includeEndpoint>* buffer;
        };


        template <uint32_t reuseableBufferSize, bool includeEndpoint>
        ReuseableBuffer<reuseableBufferSize, includeEndpoint>& ReuseablePacket<reuseableBufferSize, includeEndpoint>::data() {
            //return *(Packet<reuseableBufferSize, includeEndpoint>*)buffer->buffer;
            return *buffer;
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

    