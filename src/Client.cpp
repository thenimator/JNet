#include "Client.hpp"

using namespace JNet;

Client::Client() : udpSocket(context.getAsioContext()), udpResolver(context.getAsioContext()) {

}

Client::~Client() {
    if (!shouldDisconnect)  {
        std::cerr << "You should call disnonnect() before destroying a client object\n";
    }
}

Client::ReuseablePacket JNet::Client::getPacket() {
    return Client::ReuseablePacket(bufferManager.getBuffer());
}

void JNet::Client::sendPacket(Client::ReuseablePacket packet) {
    if (host == "") {
        if (debugFlagActive<DebugFlag::clientDebug>()) 
            std::cout << "No host given but packet send attempted\n";
        bufferManager.recycleBuffer(packet.buffer);

        return;
    }
    outgoingPackets.push(packet.buffer);

    boost::asio::post(udpSender, boost::bind(&Client::sendNextPacketToHost,this));
}

void JNet::Client::connect(const std::string &host) {
    context.async_run();
    this->host = host;
    resolveHost();
    try {
        udpSocket.connect(udpEndpoint);
        if (debugFlagActive<DebugFlag::clientDebug>()) 
            std::cout << "Conntected with: " <<
            udpEndpoint << "\n";
    } catch (boost::system::system_error& e) {
        std::cerr << e.what() << std::endl;
    }
    receive();
}

void JNet::Client::disconnect(std::chrono::microseconds finishDuration) {
    shouldDisconnect = true;
    context.shutDown(finishDuration);
    incomingPackets.clear();
    outgoingPackets.clear();
    udpSender.join();
}

bool JNet::Client::hasAvailablePacket() {
    return !incomingPackets.empty();
}

Client::ReuseablePacket JNet::Client::receiveIncomingPacket() {
    return ReuseablePacket(incomingPackets.consumeFront());
}

void JNet::Client::returnPacket(ReuseablePacket packet) {
    bufferManager.recycleBuffer(packet.buffer);
}

bool JNet::Client::hasConnection()
{
    return !shouldDisconnect;
}

void JNet::Client::receive()
{
    ReuseableBuffer* buffer = bufferManager.getBuffer();
    if (debugFlagActive<DebugFlag::clientDebug>()) {
        std::stringstream ss;
        ss << "Using buffer with id: " << buffer << " as receiveBuffer\n";
        std::cout << ss.str();
    }
    auto callback = boost::bind(&Client::handlePacketReceive, this, buffer,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred);

    udpSocket.async_receive(
    boost::asio::buffer(buffer->buffer), 
    callback
    );
}

void JNet::Client::handlePacketReceive(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, size_t messageSize) {
    if (debugFlagActive<DebugFlag::clientDebug>()) {
        std::stringstream ss;
        ss << "Started handling receive for " << messageSize << " bytes\n";
        std::cout << ss.str();
    }

    /*if (shouldClose) {
        if (debugFlagActive<DebugFlag::serverDebug>()) 
            std::cout << "Unexecuted receive due to server closing\n";
        bufferManager.recycleBuffer(recycleableBuffer);
        return;
    }*/
    receive();
    if (e.failed()) {
        std::cerr << "Error:\n" << e.message() << "\n";
        bufferManager.recycleBuffer(recycleableBuffer);
        return;
    }
    if (debugFlagActive<DebugFlag::clientDebug>()) 
        std::cout << recycleableBuffer->packet().debugString();
    incomingPackets.push(recycleableBuffer);
}

void JNet::Client::resolveHost() {
    try {
        udpEndpoint = *udpResolver.resolve(host,"16632").begin();
    } catch (boost::system::system_error& e) {
        std::cout << "Error when connecting with " << host << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Code: " << e.code() << std::endl;
    }
}

void JNet::Client::handleSentPacket(ReuseableBuffer *recycleableBuffer, const boost::system::error_code &e, std::size_t messageSize) {
    bufferManager.recycleBuffer(recycleableBuffer);
    if (e.failed()) {
        std::cerr << "Sending failed. Code: " << e.value() << "\n";
    }
}

void JNet::Client::sendNextPacketToHost() {
    ReuseableBuffer* sendBuffer = outgoingPackets.consumeFront();
    auto callBack = boost::bind(
        &Client::handleSentPacket
        ,this
        ,sendBuffer
        ,boost::asio::placeholders::error()
        ,boost::asio::placeholders::bytes_transferred()
    );
    udpSocket.async_send(boost::asio::buffer(sendBuffer->buffer),callBack);
    if (debugFlagActive<DebugFlag::clientDebug>()) {
        std::stringstream ss;

        ss << "Sent to: " << udpSocket.remote_endpoint() << "\n";
    
        std::cout << ss.str();
    }
    bufferManager.recycleBuffer(sendBuffer);
}
