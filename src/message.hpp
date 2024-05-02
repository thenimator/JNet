#pragma once
#include "defines.hpp"

namespace JNet {
    
    enum class MessageType {
        MatchmakingRequest,
        BroadcastRequest,
        MoveRequest,
        Broadcast
    };

    struct Header {
        uint64_t id = 0;
        uint32_t size = 0;
        MessageType messageType{};
    };

    class Message {
        
        friend std::ostream& operator << (std::ostream& os, const Message& msg) {
            os << "ID: " << msg.header.id << " Size: " << msg.header.size;
            return os;
        }
        /*template<typename DataType>
        friend Message& operator << (Message& msg, DataType insert) {
            
            //Is datatype trivially copyable
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be automatically pushed into the message");

            msg.body.resize(msg.body.size() + sizeof(DataType));
            *(DataType*)(msg.body.back() + 1) = insert;

            msg.header.size = msg.getSize();

            return msg;
        }*/
    public:
        Message();
        size_t getSize();
        std::vector<uint8_t> body;
        Header header;
    private:
        
        
    };

    struct ServerMessage {
        Message message;
        
    };


}