#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
//#include <boost/thread.hpp>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <concepts>
#include <functional>

namespace JNet {

    typedef uint64_t DebugFlags;

    enum DebugFlag {
        noDebug = 0x0,
        contextDebug = 0x1,
        clientDebug = 0x2,
        bufferManagerDebug = 0x4,
        serverDebug = 0x8,
        messageDebug = 0x10,
        clientMessageOutput = 0x20,
        packetDebug = 0x40
    };

    constexpr DebugFlags debugFlags = clientDebug | serverDebug | bufferManagerDebug;

    template<DebugFlag flag>
    constexpr bool debugFlagActive() {
        return (debugFlags & flag)!= 0;
    }

   

}


#define PORT 16632

#define maxMessageSize 1024

enum class BuildType {
    Client,
    Server
};