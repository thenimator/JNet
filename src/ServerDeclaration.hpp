#pragma once
#include "ClientServerBase/enums.hpp"

#define TemplatedServer Server<TPacketWrapper, TUdpReceivemode, TMessageWrapper, TTcpReceivemode>
#define TemplatedServerArgs class TPacketWrapper, udp::receiveMode TUdpReceivemode, class TMessageWrapper, tcp::receiveMode TTcpReceivemode

namespace JNet {
    template<TemplatedServerArgs>
    class Server;

}