#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"

#define BUILDTYPE BuildType::Client

int main(int argc, char** argv) {
    
    if (BUILDTYPE == BuildType::Client) {
        if (argc != 2) {
        return -1;
    }
        std::string host(argv[1]);
        Client client(host);
    }
    if (BUILDTYPE == BuildType::Server) {
        Server server;
        std::string command;
        std::cin >> command;
        server.close();
    }
}