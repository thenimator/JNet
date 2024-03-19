#include "Server.hpp"

Server::Server() : socket(context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), PORT)) {
    sender = std::thread(boost::bind(&Server::run,this));
}

void Server::run() {
    using namespace boost::asio::ip;
    try {
        while (!shouldClose) {
            std::array<bool,sizeof(Message)> receivedData;
            udp::endpoint remoteEndpoint;
            socket.receive_from(boost::asio::buffer(receivedData), remoteEndpoint);
            std::array<uint8_t, sizeof(Message)> data;
            Message message;
            message.id = (*(Header*)&receivedData).id;

            memcpy(&data,&message,sizeof(Message));

            boost::system::error_code ignoredError;
            socket.send_to(boost::asio::buffer(data),remoteEndpoint,0,ignoredError);
        }
    } catch (std::exception& e) {
        std::cerr << "What:" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "EndWhat" << std::endl;
    }
    
}

void Server::close() {
    shouldClose = true;
}

Server::~Server() {
    if (!shouldClose) {
        std::cerr << "You should call close() before destroying the object" << std::endl;
        shouldClose = true;
    }
    sender.join();
}