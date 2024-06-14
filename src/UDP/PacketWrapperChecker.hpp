#pragma once
#include "udp.hpp"

namespace JNet {
    namespace udp {
        template <class TPacketWrapper>
        class PacketWrapperChecker {
            static_assert(sizeof(TPacketWrapper) >= bufferSize, "Size of wrapper is larger than max udp packet size");
        public:
            PacketWrapperChecker() = default;
        private:
            int test;
        };
    }
}