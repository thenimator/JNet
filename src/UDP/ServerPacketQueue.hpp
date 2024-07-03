#pragma once
#include "../ClientServerBase/enums.hpp"

namespace JNet {
    namespace udp {
        template<class TPacketWrapper>
        class ServerPacketBase<TPacketWrapper, udp::receiveMode::queue> {

        };
    }
}