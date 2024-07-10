#pragma once
#include "ServerIncomingQueue.hpp"
#include "ServerIncomingCallback.hpp"
#include "../ClientServerBase/enums.hpp"

namespace JNet {
    namespace udp {
        template<class TPacketWrapper, receiveMode TReceiveMode>
        class IncomingMethod {

        };

        template<class TPacketWrapper>
        class IncomingMethod<TPacketWrapper, receiveMode::queue> : virtual public ServerIncomingQueue<TPacketWrapper> {
        public:
            IncomingMethod() : ServerIncomingQueue<TPacketWrapper>(), ServerBase<TPacketWrapper>('\0') {};
        };

        template<class TPacketWrapper>
        class IncomingMethod<TPacketWrapper, receiveMode::callback> : virtual public ServerIncomingCallback<TPacketWrapper> {
        public:
            IncomingMethod() : ServerIncomingCallback<TPacketWrapper>(), ServerBase<TPacketWrapper>('\0') {};
        };
    }
}