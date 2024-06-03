#pragma once
#include "defines.hpp"
#include <boost/asio/io_context.hpp>
#include <chrono>

namespace JNet {

    class Context {
    public:
        Context();
        ~Context();
        void async_run();
        void terminate();
        void shutDown(std::chrono::milliseconds delay = std::chrono::milliseconds(0), std::chrono::milliseconds finishTime = std::chrono::milliseconds(5000));
        const boost::asio::io_context& getAsioContext() const;  
        boost::asio::io_context& getAsioContext();
    private:
        bool running = false;
        void runContext();
        boost::asio::io_context asio_context;
        std::thread runner;
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> asioContextWorkGuard;
        

    };




}