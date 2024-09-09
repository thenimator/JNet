#pragma once
#include "../Context.hpp"
#include "../UDP/Buffer/BufferManager.hpp"
#include "../UDP/ReuseablePacket.hpp"


namespace JNet {
    template<bool TIsServer> 
    class IO_Base {
    public:
        using ReuseableBuffer = JNet::udp::ReuseableBuffer<JNet::udp::bufferSize, TIsServer>;
        using BufferManager = JNet::udp::BufferManager<JNet::udp::bufferSize, SafetyFlag::threadSafe, TIsServer>;
    public:
        IO_Base(uint16_t port);
        IO_Base(char err);
        bool isRunning();
    protected: 
        void baseRun(); 
        void baseClose(std::chrono::microseconds finishTime = std::chrono::microseconds(100));
    protected:
        JNet::Context context;
        bool shouldClose = false;

        BufferManager bufferManager;
        udp::Socket udpSocket;
        
    };

    template<bool TIsServer> 
    inline void IO_Base<TIsServer>::baseRun()
    {
        context.async_run();
    }

    template<bool TIsServer> 
    inline IO_Base<TIsServer>::IO_Base(uint16_t port) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
        if (port == 0) {
            throw(std::runtime_error("Port 0 isn't supported!"));
        }
    }

    template<bool TIsServer> 
    inline IO_Base<TIsServer>::IO_Base(char err) : udpSocket(context.getAsioContext(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 16632)) {
        if (err == 0) {
            throw std::runtime_error("Refrain from creating instances of modules. User class Server instead.");
        }
    }

    template<bool TIsServer> 
    inline bool IO_Base<TIsServer>::isRunning()
    {
        return !shouldClose;
    }

    template<bool TIsServer> 
    inline void IO_Base<TIsServer>::baseClose(std::chrono::microseconds finishTime) {
        shouldClose = true;
        context.shutDown(finishTime);
    }
}