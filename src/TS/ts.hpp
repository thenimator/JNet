#pragma once
#include "../UDP/Buffer/BufferManager.hpp"
#include "../UDP/udp.hpp"

namespace JNet {
    namespace ts {
        template <uint32_t N = udp::bufferSize>
        using BufferManager = udp::BufferManager<N, SafetyFlag::threadSafe>;
    }



}