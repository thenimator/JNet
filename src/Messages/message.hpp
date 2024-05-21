#pragma once
#include "../defines.hpp"

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
    public:
        Message();
        size_t getSize();
        //is automatically called when converting to a buffer
        void setSize();
        void writeToBuffer(void* buffer, uint32_t bufferSize);
        std::vector<uint8_t> body;
        Header header;
    private:
        
        
    };

    struct ServerMessage {
        Message message;
        //boost::asio::ip::basic_endpoint::basic_endpoint<boost::asio::ip::tcp> endpoint;
    };


    
    std::unique_ptr<Message> bufferToMessage(void* buffer);


}