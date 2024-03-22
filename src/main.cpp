#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"

#if __has_include("buildType.hpp") 
# include "buildType.hpp"
#endif

#ifndef BUILDTYPE
#define BUILDTYPE BuildType::Client
#endif


int main(int argc, char** argv) {
    
    if (BUILDTYPE == BuildType::Client) {
        if (argc != 2) {
            std::cerr << "requires exactly one argument" << std::endl;
            return -1;
        }
        std::string host(argv[1]);
        JNet::udp::Client client(host);
        client.connect();
        boost::asio::io_context context;
        boost::asio::steady_timer t(context, boost::asio::chrono::seconds(5));
        t.wait();
        client.disconnect();
        while (client.hasConnection()) {
            
        }
    }
    if (BUILDTYPE == BuildType::Server) {
        JNet::udp::Server server;
        std::string command;
        std::cin >> command;
        server.close();
    }
}