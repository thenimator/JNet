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
        boost::asio::io_context context;
        JNet::udp::Server server(context);
        std::cout << "Created server\n";
        context.run();
        std::cout << "Debug\n";
        std::string command;
        std::cin >> command;
        server.close();
        
    }
    return 0;
}