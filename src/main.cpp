#include "Messages/communication.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include <iostream>
#include "UDP/udp.hpp"


#if __has_include("buildType.hpp") 
# include "buildType.hpp"
#endif

#ifndef BUILDTYPE
#define BUILDTYPE BuildType::Client
#endif

void serverQueuePacketPrinter(JNet::Server<JNet::udp::Packet<>, JNet::udp::receiveMode::queue, JNet::udp::Packet<>, JNet::tcp::receiveMode::queue>* server) {
    std::cout << "Packetprinter called\n";
    while(server->isRunning()) {
        if (server->hasAvailablePacket()) {
            JNet::udp::ReuseablePacket packet = server->receiveIncomingPacket();
            std::cout << packet.wrapper().debugString() << "\n";
            std::string response = "Hello there random stranger!";
            memcpy(packet.wrapper().getData(),response.data(),response.size());
            packet.wrapper().setSize(response.size());
            server->sendPacket(std::move(packet));
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(500));
    }
    std::cout << "Exiting packetprinter\n";
}

void clientQueuePacketPrinter(JNet::Client<JNet::udp::Packet<>, JNet::udp::receiveMode::queue, JNet::udp::Packet<>, JNet::tcp::receiveMode::queue>* client) {
    std::cout << "Packetprinter called\n";
    while(client->hasConnection()) {
        if (client->hasAvailablePacket()) {
            JNet::udp::ReuseablePacket packet = client->receiveIncomingPacket();
            std::cout << packet.wrapper().debugString() << "\n";
            client->returnPacket(std::move(packet));
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(500));
    }
}


int clientQueueTest(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "requires exactly one argument" << std::endl;
        return -1;
    }
    std::string host(argv[1]);
    std::string port = "16632";
    JNet::Client<JNet::udp::Packet<>, JNet::udp::receiveMode::queue, JNet::udp::Packet<>, JNet::tcp::receiveMode::queue> client;
    client.connect(host, port);
    uint32_t messageCount = 0;
    std::string YES = "Hello world!";
    std::thread packetReceiver = std::thread(boost::bind(&clientQueuePacketPrinter, &client));
    while (messageCount < 1) {
        JNet::udp::ReuseablePacket<JNet::udp::Packet<>, JNet::udp::bufferSize> packet = std::move(client.getPacket());
        memcpy(packet.wrapper().getData(),YES.data(),YES.size());
        packet.wrapper().setSize(YES.size());
        packet.wrapper().setMessageType(JNet::MessageType::Unset);
        packet.wrapper().setId(messageCount);
        client.sendPacket(std::move(packet));
        messageCount++;
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    client.disconnect();
    packetReceiver.join();
    return 0;
}

void serverQueueTest() {
    JNet::Server<JNet::udp::Packet<>, JNet::udp::receiveMode::queue, JNet::udp::Packet<>, JNet::tcp::receiveMode::queue> server(16632);
    std::cout << "Created server\n";
    bool running = true;
    server.run();
    std::thread packetReceiver = std::thread(boost::bind(&serverQueuePacketPrinter, &server));
    while (running) {
        std::string command;
        std::cin >> command;
        if (command == "close") {
            running = false;
        }
    }

    
    server.close();
    packetReceiver.join();
}

class ServerCallback {
public:
    void callback(JNet::udp::ReuseablePacket<JNet::udp::Packet<>, JNet::udp::bufferSize, true> packet) {
        std::cout << packet.wrapper().debugString();
        std::string response = "Hello there random stranger!";
        memcpy(packet.wrapper().getData(),response.data(),response.size());
        packet.wrapper().setSize(response.size());
        server->sendPacket(std::move(packet));

    }
public:
    JNet::Server<JNet::udp::Packet<>, JNet::udp::receiveMode::callback, JNet::udp::Packet<>, JNet::tcp::receiveMode::queue>* server;
};

void serverCallbackTest() {
    JNet::Server<JNet::udp::Packet<>, JNet::udp::receiveMode::callback, JNet::udp::Packet<>, JNet::tcp::receiveMode::queue> server(16632);
    std::cout << "Created server\n";
    ServerCallback callbackObj;
    callbackObj.server = &server;
    bool running = true;
    auto callback = std::bind(&ServerCallback::callback, &callbackObj, std::placeholders::_1);
    server.setCallback(callback);
    server.run();
    while (running) {
        std::string command;
        std::cin >> command;
        if (command == "close") {
            running = false;
        }
    }

    
    server.close();
}

int main(int argc, char** argv) {
    if (BUILDTYPE == BuildType::Client) {       
        return clientQueueTest(argc, argv);
    }
    if (BUILDTYPE == BuildType::Server) {
        serverCallbackTest();
    }
    return 0;
}