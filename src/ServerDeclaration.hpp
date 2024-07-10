#pragma once
#include "ClientServerBase/enums.hpp"
#include "UDP/ServerIncomingQueueDeclaration.hpp"
#include "UDP/ServerIncomingCallbackDeclaration.hpp"
#include "UDP/ServerOutgoingQueueDeclaration.hpp"

#define TemplatedServer Server<TPacketWrapper, TUdpReceivemode, TMessageWrapper, TTcpReceivemode>
#define TemplatedServerArgs class TPacketWrapper, udp::receiveMode TUdpReceivemode, class TMessageWrapper, tcp::receiveMode TTcpReceivemode

namespace JNet {
    template<TemplatedServerArgs>
    class Server;



}