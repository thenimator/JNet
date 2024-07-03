#pragma once
#include "defines.hpp"
#include "TS/queue.hpp"
#include "Messages/message.hpp"
#include "Context.hpp"
#include "UDP/packet.hpp"
#include "UDP/udp.hpp"
#include "UDP/Buffer/BufferManager.hpp"
#include "UDP/ReuseablePacket.hpp"
#include "UDP/PacketWrapperChecker.hpp"
#include "TCP/Connection.hpp"
#include "ClientServerBase/enums.hpp"
#include "shorteners.hpp"
#include "UDP/ServerIncomingQueue.hpp"

namespace JNet {

    

    
    template<TemplatedServerArgs>
    class Server : private udp::PacketWrapperChecker<TPacketWrapper>, public udp::ServerIncomingQueue<TPacketWrapper> {
    public:
        using UDPTYPES;
        using TCPTYPES;
        
    public:
        Server(uint16_t port);
        void run();
        void close(std::chrono::microseconds finishTime = std::chrono::microseconds(100));
        ~Server();
    private:
        void acceptConnections();
        void acceptConnection();
        void handleConnectionAccept(Connection* connection, const boost::system::error_code& e);
    private:
        //boost::asio::ip::tcp::acceptor acceptor;



    };

/*
    template<TemplatedServerArgs>
    void TemplatedServer::handleConnectionAccept(Connection* connection, const boost::system::error_code& e) {
        acceptConnection();




    }*/


    template<TemplatedServerArgs>
    TemplatedServer::Server(uint16_t port) : udp::ServerIncomingQueue<TPacketWrapper>(port)
        //, acceptor(context.getAsioContext(), boost::asio::ip::cleatcp::endpoint(boost::asio::ip::tcp::v4(), port)) 
        {
        //boost::asio::ip::tcp::endpoint endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
        //acceptor.open(endpoint.protocol());
        //acceptor.bind(endpoint);
        //acceptor.listen();
        //std::cout << "Constructed server\n";
    }

    template<TemplatedServerArgs>
    void TemplatedServer::run() {
        this->baseRun();
        this->runUdpReceive();
        acceptConnections();
    }

    template<TemplatedServerArgs>
    void TemplatedServer::acceptConnections() { 
        acceptConnection();
    }


    template<TemplatedServerArgs>
    void TemplatedServer::acceptConnection() { 
        //Connection* openConnection = Connection::create(context);

        //auto callback = boost::bind(&Server::handleConnectionAccept, this, openConnection, boost::asio::placeholders::error);

        //acceptor.async_accept(openConnection->getSocket(), callback);
    }

    template<TemplatedServerArgs>
    void TemplatedServer::close(std::chrono::microseconds finishTime) {
        this->baseClose();
        this->udpReceiverClose();
    }

    //should perhaps be moved
    template<TemplatedServerArgs>
    TemplatedServer::~Server() {
        if (!this->shouldClose) {
            std::cerr << "You should call close() before destroying a server object\n";
            close();
        }
    }
}

