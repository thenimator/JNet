#pragma once
#include "../defines.hpp"

namespace JNet {
    typedef uint32_t safetyFlags;

    enum SafetyFlag {
        unSafe = 0,
        threadSafe = 1
    };


    template<SafetyFlag flag>
    constexpr bool hasFlag(safetyFlags flags) {
        return (flags & flag)!= 0;
    }
}