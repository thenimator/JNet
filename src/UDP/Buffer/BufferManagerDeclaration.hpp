#pragma once
#include "../udp.hpp"
#include "../../defines.hpp"
#include "../../Flags/flags.hpp"

namespace JNet {
    namespace udp {
        template <uint32_t bufferSize = bufferSize, safetyFlags flags = 0, bool includeEndpoint = false>
        class BufferManager;
    } 
}