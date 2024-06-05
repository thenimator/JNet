#include "Server.hpp"
#include <iostream>

using namespace JNet;

Server::Server(JNet::Context& context) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), PORT)) {

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
    /*Message response; 
    response.header.id = messageCount;
    response.header.messageType = MessageType::Broadcast;
    std::string answer = "Data could be here";
    response.header.size = answer.size();
    response.body.resize(answer.size());
    memcpy(response.body.data(),answer.data(),answer.size());
    messageCount++;
    socket.async_send_to(boost::asio::buffer(*(std::array<char,1024>*)&response),remoteEndpoint,std::bind(&Server::handleMessage, this, e, messageSize));*/

}

void Server::receive() {
    ReuseableBuffer* buffer = bufferManager.getBuffer();


    udpSocket.async_receive_from(
    boost::asio::buffer(buffer->buffer), 
    buffer->endpoint, 
    boost::bind(&Server::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
    );
    if (debugFlagActive<DebugFlag::serverDebug>()) 
        std::cout << "Receiving from: " <<
        udpSocket.local_endpoint() << "\n";


    if (debugFlagActive<DebugFlag::serverDebug>()) {
        if (udpSocket.is_open()) {
            std::cout << "Socket is open\n";
        } else {
            std::cout << "Socket is closed\n";
        }
        
    }
}

void Server::handlePacketReceive(ReuseableBuffer* recycleableBuffer,const boost::system::error_code& e, size_t messageSize) {
    if (debugFlagActive<DebugFlag::serverDebug>()) {
        std::stringstream ss;
        ss << "Started handling receive for " << messageSize << " bytes\n";
        std::cout << ss.str();
    }
        
    if (!shouldClose) {
        if (!e.failed()) {
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Hasn't failed" << std::endl;
            udp::Packet<>& packet = reinterpret_cast<udp::Packet<>&>(recycleableBuffer->buffer);
            //messages.push(receivedMessage);
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << "Aquired message" << std::endl;
            //receive shhould be moved back here
            //receive();
            respond(e, messageSize);
            if (debugFlagActive<DebugFlag::serverDebug>()) 
                std::cout << packet.debugString();
            receive();
            return;
        }

        std::cerr << "Error:\n" << e.message() << std::endl;
    }
    
    if (debugFlagActive<DebugFlag::serverDebug>()) 
        std::cout << "Unexecuted receive\n";


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