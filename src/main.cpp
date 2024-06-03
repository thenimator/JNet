#include "Messages/communication.hpp"
#include "UDP/Client.hpp"
#include "UDP/Server.hpp"
#include "defines.hpp"
#include <iostream>

#if __has_include("buildType.hpp") 
# include "buildType.hpp"
#endif

#ifndef BUILDTYPE
#define BUILDTYPE BuildType::Client
#endif


int main(int argc, char** argv) {
    JNet::Context context;
    if (BUILDTYPE == BuildType::Client) {
        context.async_run();
        if (argc != 2) {
            std::cerr << "requires exactly one argument" << std::endl;
            return -1;
        }
        std::string host(argv[1]);
        JNet::udp::Client client(context);
        client.connect(host);
        JNet::udp::Packet packet;
        uint32_t messageCount = 0;
        while (messageCount < 1) {
            client.sendPacket(packet);
            messageCount++;
        }
    }
    if (BUILDTYPE == BuildType::Server) {
        JNet::udp::Server server(context);
        std::cout << "Created server\n";
        server.run();
        bool running = true;
        context.async_run();
        while (running) {
            std::string command;
            std::cin >> command;
            if (command == "close") {
                running = false;
            }
        }
        
        server.close();
        
    }
    context.shutDown(std::chrono::milliseconds(100));
    return 0;
}