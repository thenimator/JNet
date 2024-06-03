#pragma once
#include "../defines.hpp"

namespace JNet {
    
    enum class MessageType {
        Unset,
        MatchmakingRequest,
        BroadcastRequest,
        MoveRequest,
        Broadcast
    };

    struct Header {
        uint64_t id = 0;
        uint32_t size = 0;
        MessageType messageType = MessageType::Unset;
    };

    



    
    //std::unique_ptr<Message> bufferToMessage(void* buffer);


}