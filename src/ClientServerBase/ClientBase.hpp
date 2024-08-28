#pragma once
#include "../Context.hpp"
#include "../UDP/Buffer/BufferManager.hpp"
#include "../UDP/ReuseablePacket.hpp"

namespace JNet {
    class ClientBase {
    public:
        using ReuseableBuffer = JNet::udp::ReuseableBuffer<JNet::udp::bufferSize,true>;
        using BufferManager = JNet::udp::BufferManager<JNet::udp::bufferSize, SafetyFlag::threadSafe, true>;
    public:
        ClientBase(uint16_t port);
        ClientBase(char err);
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

    inline void ClientBase::baseRun()
    {
        context.async_run();
    }

    inline ClientBase::ClientBase(uint16_t port) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
        if (port == 0) {
            throw(std::runtime_error("Port 0 isn't supported!"));
        }
    }

    inline ClientBase::ClientBase(char err) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 16632)) {
        if (err == 0) {
            throw std::runtime_error("Refrain from creating instances of modules. User class Server instead.");
        }
    }

    inline bool ClientBase::isRunning()
    {
        return !shouldClose;
    }

    inline void ClientBase::baseClose(std::chrono::microseconds finishTime) {
        shouldClose = true;
        context.shutDown(finishTime);
    }
}