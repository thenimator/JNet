#pragma once
#include "ClientServerBase/enums.hpp"

#define TemplatedClient Client<TPacketWrapper, TUdpReceivemode, TMessageWrapper, TTcpReceivemode>
#define TemplatedClientArgs class TPacketWrapper, udp::receiveMode TUdpReceivemode, class TMessageWrapper, tcp::receiveMode TTcpReceivemode

namespace JNet {
    template<TemplatedClientArgs>
    class Client;
} 
