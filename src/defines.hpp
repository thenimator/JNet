#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <queue>


#define PORT 16632

#define maxMessageSize 1024

enum class MessageType {
    MatchmakingRequest,
    BroadcastRequest,
    MoveRequest,
    Broadcast
};

struct Header {
    uint64_t id;
    uint32_t messageLength;
    MessageType messageType;
};
struct Message {
    Header header;
    std::array<uint8_t, maxMessageSize - sizeof(Header)> data;
};
enum class BuildType {
    Client,
    Server
};