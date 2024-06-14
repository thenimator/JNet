#pragma once
#include "../udp.hpp"
#include "../../defines.hpp"
#include "../../Flags/flags.hpp"

namespace JNet {
    namespace udp {
        template<bool c>
        struct ReuseableBufferBase { };

        template<>
        struct ReuseableBufferBase<true> {
            Endpoint endpoint;
        };



        template <uint32_t reuseableBufferSize = bufferSize, bool TIncludeEndpoint = false>
        class ReuseableBuffer : public ReuseableBufferBase<TIncludeEndpoint>{
            template <uint32_t bufferSize, safetyFlags flags, bool includeEndpoint>
            friend class BufferManager;
        public:  
            std::array<uint8_t, reuseableBufferSize> buffer;
        protected:
            ReuseableBuffer* next;

            
        };
    }
}