#include "Client.hpp"

using namespace JNet;

Client::Client(Context &givenContext) : context(givenContext), udpSocket(givenContext.getAsioContext()), udpResolver(context.getAsioContext()) {

}

Client::~Client() {
    udpSender.join();
}

udp::ReuseablePacket<> JNet::Client::getPacket() {
    return udp::ReuseablePacket<>(bufferManager.getBuffer());
}

void JNet::Client::sendPacket(udp::ReuseablePacket<> packet) {
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
}

std::unique_ptr<udp::Packet<>> JNet::Client::receiveMessage() {
    return std::unique_ptr<udp::Packet<>>();
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
}
