#include "message.hpp"

using namespace JNet;

size_t Message::getSize () {
    return body.size() + sizeof(Header);
}

Message::Message() {
    
}

void Message::setSize() {
    header.size = sizeof(header) + body.size();
}

void Message::writeToBuffer(void* buffer, uint32_t bufferSize) {
    uint32_t pasteSize = sizeof(Header);

    //so the user doesn't have to call this manually
    this->setSize();
    
    if (pasteSize < bufferSize)
        return;
    *(uint64_t*)buffer = this->header.id;
    buffer = ((uint8_t*)buffer) + sizeof(uint64_t);
    *(JNet::MessageType*)buffer = this->header.messageType;
    buffer = ((uint8_t*)buffer) + sizeof(JNet::MessageType);
    *(uint32_t*)buffer = this->header.size;
    buffer = ((uint8_t*)buffer) + sizeof(uint32_t);

    bufferSize -= sizeof(Header);

    uint32_t copySize = std::min<uint32_t>(bufferSize,this->body.size());

    memcpy(buffer,this->body.data(),copySize);

}

std::unique_ptr<Message> JNet::bufferToMessage(void* buffer) {
    std::unique_ptr<Message> returnMessage;
    returnMessage->header.id = *(uint64_t*)buffer;
    buffer = ((uint8_t*)buffer) + sizeof(uint64_t);
    returnMessage->header.messageType = *(JNet::MessageType*)buffer;
    buffer = ((uint8_t*)buffer) + sizeof(JNet::MessageType);
    returnMessage->header.size = *(uint32_t*)buffer;
    buffer = ((uint8_t*)buffer) + sizeof(uint32_t);

    returnMessage->body.reserve(returnMessage->header.size - sizeof(Header));
    memcpy(returnMessage->body.data(), buffer, returnMessage->header.size - sizeof(Header));

    return returnMessage;
}
