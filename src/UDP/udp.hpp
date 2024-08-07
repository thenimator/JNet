#pragma once
#include "../defines.hpp"

namespace JNet {
    namespace udp {
        constexpr uint32_t udpHeaderSize = 0x1d;
        constexpr uint32_t bufferSize = 0x10000 - udpHeaderSize;
        using Endpoint = boost::asio::ip::basic_endpoint<boost::asio::ip::udp>;
        using Socket = boost::asio::basic_datagram_socket<boost::asio::ip::udp>;
        using Resolver = boost::asio::ip::basic_resolver<boost::asio::ip::udp>;
        //using ServerReuseableBuffer = udp::ReuseableBuffer<JNet::udp::bufferSize,true>; 
        //using ServerBufferManager = udp::BufferManager<JNet::udp::bufferSize, SafetyFlag::threadSafe, true>; 
        //using ServerReuseablePacket = JNet::udp::ReuseablePacket<TPacketWrapper ,JNet::udp::bufferSize, true>;
    }
} 

