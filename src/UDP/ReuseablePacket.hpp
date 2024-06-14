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
        template <class TWrapper, uint32_t reuseableBufferSize = bufferSize, bool includeEndpoint = false>
        class ReuseablePacket {
            template<class TPacketWrapper>
            friend class JNet::Client;
            template<class TPacketWrapper>
            friend class JNet::Server;
        public:
            ReuseablePacket() = delete;
            ReuseablePacket(const ReuseablePacket<TWrapper, reuseableBufferSize, includeEndpoint>& other) = delete;
            ReuseablePacket(const ReuseablePacket<TWrapper, reuseableBufferSize, includeEndpoint>&& other);
            ReuseablePacket<TWrapper, reuseableBufferSize,includeEndpoint>& operator= (const ReuseablePacket<TWrapper, reuseableBufferSize, includeEndpoint>& other) = delete;
        public:
            ReuseableBuffer<reuseableBufferSize, includeEndpoint>& data();
            TWrapper& wrapper();
        private:
            ReuseablePacket(ReuseableBuffer<reuseableBufferSize, includeEndpoint>* buffer);
        private:
            ReuseableBuffer<reuseableBufferSize, includeEndpoint>* buffer;
        };

        template<class TWrapper, uint32_t reuseableBufferSize, bool includeEndpoint>
        TWrapper& ReuseablePacket<TWrapper, reuseableBufferSize, includeEndpoint>::wrapper() {
            return *(TWrapper*)buffer;
        }


        template <class TWrapper,uint32_t reuseableBufferSize, bool includeEndpoint>
        ReuseableBuffer<reuseableBufferSize, includeEndpoint>& ReuseablePacket<TWrapper, reuseableBufferSize, includeEndpoint>::data() {
            //return *(Packet<reuseableBufferSize, includeEndpoint>*)buffer->buffer;
            return *buffer;
        }

        template <class TWrapper, uint32_t reuseableBufferSize, bool includeEndpoint>
        ReuseablePacket<TWrapper, reuseableBufferSize, includeEndpoint>::ReuseablePacket(ReuseableBuffer<reuseableBufferSize, includeEndpoint>* buffer) {
            this->buffer = buffer;
        }

        template <class TWrapper, uint32_t reuseableBufferSize, bool includeEndpoint>
        ReuseablePacket<TWrapper, reuseableBufferSize, includeEndpoint>::ReuseablePacket(const ReuseablePacket<TWrapper, reuseableBufferSize, includeEndpoint>&& other) {
            buffer = other.buffer;
        }
    }
}

    