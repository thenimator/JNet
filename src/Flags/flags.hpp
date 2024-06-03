#pragma once
#include "../defines.hpp"

namespace JNet {
    typedef uint32_t safetyFlags;

    enum SafetyFlag {
        unSafe = 0x0,
        threadSafe = 0x1,
        runtimeBoundsChecks = 0x2
    };


    template<SafetyFlag flag>
    constexpr bool hasFlag(safetyFlags flags) {
        return (flags & flag)!= 0;
    }
}