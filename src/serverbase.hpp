#pragma once
#include "Context.hpp"
#include "UDP/Buffer/BufferManager.hpp"
#include "shorteners.hpp"

namespace JNet {
    template<class TPacketWrapper>
    class ServerBase {
    public:
        using UDPTYPES;
    public:
        ServerBase(uint16_t port);
        ServerBase(char err);
        bool isRunning();
        /** gets and empty packet which can be send using this server
            */        
        ReuseablePacket getEmptyPacket();
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

    template <class TPacketWrapper>
    inline typename ServerBase<TPacketWrapper>::ReuseablePacket ServerBase<TPacketWrapper>::getEmptyPacket() {
        return ReuseablePacket(this->bufferManager.getBuffer());
    }

    template <class TPacketWrapper>
    inline void ServerBase<TPacketWrapper>::baseRun()
    {
        context.async_run();
    }

    template <class TPacketWrapper>
    inline ServerBase<TPacketWrapper>::ServerBase(uint16_t port) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
        if (port == 0) {
            throw(std::runtime_error("Port 0 isn't supported!"));
        }
    }

    template <class TPacketWrapper>
    inline ServerBase<TPacketWrapper>::ServerBase(char err) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 16632)) {
        if (err == 0) {
            throw std::runtime_error("Refrain from creating instances of modules. User class Server instead.");
        }
    }

    template <class TPacketWrapper>
    inline bool ServerBase<TPacketWrapper>::isRunning()
    {
        return !shouldClose;
    }

    template<class TPacketWrapper>
    inline void ServerBase<TPacketWrapper>::baseClose(std::chrono::microseconds finishTime) {
        shouldClose = true;
        context.shutDown(finishTime);
    }
}