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
        /** @brief shuts down the JNet::Context
         * @brief is guaranteed to wait for at least finishTIme for outstanding operations
        */
        void shutDown(std::chrono::microseconds finishTime = std::chrono::microseconds(100), std::chrono::microseconds delay = std::chrono::microseconds(0));
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