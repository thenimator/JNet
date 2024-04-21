#include "message.hpp"

using namespace JNet;

size_t Message::getSize () {
    return body.size() + sizeof(Header);
}

Message::Message() {
    
}

