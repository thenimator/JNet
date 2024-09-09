#pragma once
#include "debugMode.hpp"

namespace JNet {
    template<DebugFlag flag>
    constexpr bool debugFlagActive() {
        return (debugFlags & flag)!= 0;
    }

}