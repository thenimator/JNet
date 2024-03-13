#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <queue>

#define PORT 16632



struct Header {
    uint64_t id;
};
struct BroadcastRequest {
    Header header;
};
struct Message {
    uint64_t id;
    std::array<uint8_t, 35> data;
};
enum class BuildType {
    Client,
    Server
};