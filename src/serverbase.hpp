#pragma once
#include "Context.hpp"

namespace JNet {
    class ServerBase {
    public:
        bool isRunning();
    protected: 
        void baseRun(); 
        void baseClose(std::chrono::microseconds finishTime = std::chrono::microseconds(100));
    protected:
        JNet::Context context;
        bool shouldClose = false;
        
    };


    void ServerBase::baseRun() {
        context.async_run();
    }

    inline bool ServerBase::isRunning() {
        return !shouldClose;
    }

    inline void ServerBase::baseClose(std::chrono::microseconds finishTime) {
        shouldClose = true;
        context.shutDown(finishTime);
    }
}