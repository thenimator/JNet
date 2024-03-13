#pragma once
#include "defines.hpp"


class Client {
public:
    Client(const std::string& host);
    void connect();
    void receiveData();
    ~Client();
private:
    bool shouldDisconnect = false;
    bool activeConnection = false;
    boost::asio::io_context context;
    std::thread receiver;
    std::queue<Message> messages;
    boost::asio::ip::udp::endpoint endpoint;
    uint64_t messageCount = 0;

};

