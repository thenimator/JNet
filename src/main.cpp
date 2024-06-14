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


void packetPrinter(JNet::Server<JNet::udp::Packet<>>* server) {
    std::cout << "Packetprinter called\n";
    while(server->isRunning()) {
        if (server->hasAvailablePacket()) {
            JNet::udp::ReuseablePacket packet = server->receiveIncomingPacket();
            std::cout << packet.data().wrapper().debugString() << "\n";
            std::string response = "Hello there random stranger!";
            memcpy(packet.data().wrapper().getData(),response.data(),response.size());
            packet.data().wrapper().setSize(response.size());
            server->sendPacket(std::move(packet));
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(500));
    }
    std::cout << "Exiting packetprinter\n";
}

void clientPacketPrinter(JNet::Client<JNet::udp::Packet<>>* client) {
    std::cout << "Packetprinter called\n";
    while(client->hasConnection()) {
        if (client->hasAvailablePacket()) {
            JNet::udp::ReuseablePacket packet = client->receiveIncomingPacket();
            std::cout << packet.data().wrapper().debugString() << "\n";
            client->returnPacket(std::move(packet));
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(500));
    }
}



int main(int argc, char** argv) {
    if (BUILDTYPE == BuildType::Client) {
        if (argc != 2) {
            std::cerr << "requires exactly one argument" << std::endl;
            return -1;
        }
        std::string host(argv[1]);
        std::string port = "16632";
        JNet::Client<JNet::udp::Packet<>> client;
        client.connect(host, port);
        uint32_t messageCount = 0;
        std::string YES = "Hello world!";
        std::thread packetReceiver = std::thread(boost::bind(&clientPacketPrinter, &client));
        while (messageCount < 1) {
            JNet::udp::ReuseablePacket<JNet::udp::Packet<>, JNet::udp::bufferSize> packet = std::move(client.getPacket());
            memcpy(packet.data().wrapper().getData(),YES.data(),YES.size());
            packet.data().wrapper().setSize(YES.size());
            packet.data().wrapper().setMessageType(JNet::MessageType::Unset);
            packet.data().wrapper().setId(messageCount);
            client.sendPacket(std::move(packet));
            messageCount++;
            std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
        }
        client.disconnect();
        packetReceiver.join();
    }
    if (BUILDTYPE == BuildType::Server) {
        JNet::Server<JNet::udp::Packet<>> server(16632);
        std::cout << "Created server\n";
        bool running = true;
        server.run();
        std::thread packetReceiver = std::thread(boost::bind(&packetPrinter, &server));
        std::cout << "Test\n";
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
    return 0;
}