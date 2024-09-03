#pragma once
#include <cstdint>


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
}
