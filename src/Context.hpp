#pragma once
#include "defines.hpp"
#include <boost/asio/io_context.hpp>

namespace JNet {

    class Context {
    public:
        Context();
        ~Context();
        void async_run();
        void shutDown();
        const boost::asio::io_context& getAsioContext() const;  
        boost::asio::io_context& getAsioContext();
    private:
        void runContext();
        boost::asio::io_context asio_context;
        std::thread runner;
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> asioContextWorkGuard;
        

    };




}