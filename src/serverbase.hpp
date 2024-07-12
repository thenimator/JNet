#pragma once
#include "Context.hpp"
#include "UDP/Buffer/BufferManager.hpp"
#include "UDP/ReuseablePacket.hpp"


namespace JNet {
    class ServerBase {
    public:
        using ReuseableBuffer = JNet::udp::ReuseableBuffer<JNet::udp::bufferSize,true>;
        using BufferManager = JNet::udp::BufferManager<JNet::udp::bufferSize, SafetyFlag::threadSafe, true>;
    public:
        ServerBase(uint16_t port);
        ServerBase(char err);
        bool isRunning();
    protected: 
        void baseRun(); 
        void baseClose(std::chrono::microseconds finishTime = std::chrono::microseconds(100));
    protected:
        JNet::Context context;
        bool shouldClose = false;

        BufferManager bufferManager;
        udp::Socket udpSocket;
        udp::Endpoint udpEndpoint;
        
    };

    inline void ServerBase::baseRun()
    {
        context.async_run();
    }

    inline ServerBase::ServerBase(uint16_t port) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
        if (port == 0) {
            throw(std::runtime_error("Port 0 isn't supported!"));
        }
    }

    inline ServerBase::ServerBase(char err) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 16632)) {
        if (err == 0) {
            throw std::runtime_error("Refrain from creating instances of modules. User class Server instead.");
        }
    }

    inline bool ServerBase::isRunning()
    {
        return !shouldClose;
    }

    inline void ServerBase::baseClose(std::chrono::microseconds finishTime) {
        shouldClose = true;
        context.shutDown(finishTime);
    }
}