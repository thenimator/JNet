#include "Server.hpp"
#include <iostream>

using namespace JNet;

Server::Server() : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), PORT)) {

}

void Server::run() {
    receive();
    context.async_run();
}

void Server::receive() {
    ReuseableBuffer* buffer = bufferManager.getBuffer();
    auto callback = boost::bind(&Server::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred);

    udpSocket.async_receive_from(
    boost::asio::buffer(buffer->buffer), 
    buffer->endpoint, 
    callback
    );
}

void Server::handlePacketReceive(ReuseableBuffer* recycleableBuffer,const boost::system::error_code& e, size_t messageSize) {
    if (shouldClose) {
        if (debugFlagActive<DebugFlag::serverDebug>()) 
            std::cout << "Unexecuted receive due to server closing\n";
        bufferManager.recycleBuffer(recycleableBuffer);
        return;
    }
    receive();
    if (e.failed()) {
        std::cerr << "Error:\n" << e.message() << "\n";
        bufferManager.recycleBuffer(recycleableBuffer);
        return;
    }
    
    
    if (debugFlagActive<DebugFlag::serverDebug>()) 
        std::cout << "Handling received packet" << "\n";


    
    //if (debugFlagActive<DebugFlag::serverDebug>()) 
    //    std::cout << recycleableBuffer->packet().debugString();
    incomingPackets.push(recycleableBuffer);
    
}

void JNet::Server::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize) {
    if (e.failed()) {
        std::stringstream ss;
        ss << "Error when responding:\n" << e.message() << "\n";
        std::cerr << ss.str();     
    }
    if (debugFlagActive<DebugFlag::serverDebug>()) 
        std::cout << "Successfully responded!\n";
    bufferManager.recycleBuffer(recycleableBuffer);
}

void JNet::Server::sendNextPacket() {
    ReuseableBuffer* sendBuffer = outgoingPackets.consumeFront();
    auto callBack = boost::bind(
        &Server::handleSentPacket
        ,this
        ,sendBuffer
        ,boost::asio::placeholders::error()
        ,boost::asio::placeholders::bytes_transferred()
    );
    udpSocket.async_send_to(boost::asio::buffer(sendBuffer->buffer),sendBuffer->endpoint,callBack);
}

void Server::close(std::chrono::microseconds finishTime) {
    shouldClose = true;
    context.shutDown(finishTime);
    outgoingPackets.clear();
    incomingPackets.clear();
    udpSender.join();
}

Server::ReuseablePacket Server::getEmptyPacket() {
    return ReuseablePacket(bufferManager.getBuffer());
}

void JNet::Server::sendPacket(ReuseablePacket packet) {
    outgoingPackets.push(packet.buffer);

    boost::asio::post(udpSender, boost::bind(&Server::sendNextPacket,this));
}

void JNet::Server::setPacketCallback(void (*callback)()) {
    this->callback = callback;
}

bool JNet::Server::hasAvailablePacket() {
    return !incomingPackets.empty();
}

Server::ReuseablePacket JNet::Server::receiveIncomingPacket() {
    return ReuseablePacket(incomingPackets.consumeFront());
}

void JNet::Server::returnPacket(ReuseablePacket packet) {
    bufferManager.recycleBuffer(packet.buffer);
}

bool JNet::Server::isRunning() {
    return !shouldClose;
}

Server::~Server() {
    if (!shouldClose) {
        std::cerr << "You should call close() before destroying a server object\n";
        close();
    }
}