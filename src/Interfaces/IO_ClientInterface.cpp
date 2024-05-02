#include "IO_ClientInterface.hpp"


using namespace JNet;

void IO_ClientInterface::sendMessage(std::unique_ptr<Message> message) {
    outgoing.push(message);
}

std::unique_ptr<Message> IO_ClientInterface::receiveMessage() {
    std::unique_ptr<Message> returnval = std::move(incoming.front());
    incoming.pop();
    return returnval;

}