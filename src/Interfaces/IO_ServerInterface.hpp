#pragma once
#include "../TS/queue.hpp"
#include "../message.hpp"
#include "../defines.hpp"

namespace JNet {
    class IO_ServerInterface {
    public:
        void sendMessage(std::unique_ptr<Message> message);
        std::unique_ptr<Message> receiveMessage();
    protected:
        ts::Queue<std::unique_ptr<Message>> incoming;
        ts::Queue<std::unique_ptr<Message>> outgoing;
    };
}