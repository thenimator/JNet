#pragma once
#include "../DataStructures/BufferManager.hpp"

namespace JNet {
    namespace ts {
        template <uint32_t N>
        using BufferManager = data::BufferManager<N, SafetyFlag::threadSafe>;
    }



}