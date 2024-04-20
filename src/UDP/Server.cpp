#include "Server.hpp"
#include <iostream>

using namespace JNet::udp;

Server::Server(boost::asio::io_context& context) : socket(context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), PORT)) {
    run();
}

void Server::run() {
    /*try {
        while (!shouldClose) {
            std::array<bool,sizeof(Message)> receivedData;
            ip::udp::endpoint remoteEndpoint;
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
    }*/
    receive();
    
}

void Server::handleMessage(const boost::system::error_code& e, size_t messageSize) {
    
}

void Server::respond(const boost::system::error_code& e, size_t messageSize) {
    Message response; 
    response.header.id = messageCount;
    response.header.messageType = MessageType::Broadcast;
    std::string answer = "Data could be here";
    response.header.messageLength = answer.size();
    memcpy(&response.data,answer.data(),answer.size());
    messageCount++;
    socket.async_send_to(boost::asio::buffer(*(std::array<char,1024>*)&response),remoteEndpoint,std::bind(&Server::handleMessage, this, e, messageSize));

}

void Server::receive() {
    socket.async_receive_from(
    boost::asio::buffer(receiveBuffer.buffer), 
    remoteEndpoint, 
    boost::bind(&Server::handleReceive, this, boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
}

void Server::handleReceive(const boost::system::error_code& e, size_t messageSize) {
    if (!shouldClose) {
        if (!e.failed()) {
            std::unique_lock<std::mutex> queueLock {messagesMutex, std::defer_lock};
            queueLock.lock();
            messages.push(receiveBuffer.message);
            queueLock.unlock();
            receive();
            respond(e, messageSize);
            return;
        }

        std::cerr << "Error:\n" << e.message() << std::endl;
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
}