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
    
    Context::Context() : asioContextWorkGuard(boost::asio::make_work_guard(asio_context)) {

    }

    Context::~Context() {
        if (debugFlagActive<DebugFlag::contextDebug>()) 
            if (running) {
                std::cerr << "Running JNet::Context terminating due to destructor call.\n" 
                << "Always manually shutDown/terminate a JNet::Context\n";
                terminate();
            }
                

    }

    void Context::async_run() {
        
        runContext();
    }

    void Context::shutDown(std::chrono::microseconds finishTime, std::chrono::microseconds delay ) {
        if (debugFlagActive<DebugFlag::contextDebug>()) 
            std::cout << "Shutdown called with " << finishTime.count() << " microseconds as finishTime\n";
        if (!running) {
            if (debugFlagActive<DebugFlag::contextDebug>()) 
                std::cerr << "Context::shutDown() called while context isn't running\n";
            return;
        }
        

        std::this_thread::sleep_for(delay);
        
        asioContextWorkGuard.reset();
        while (!asio_context.stopped())  {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            finishTime -= std::chrono::microseconds(1);
            if (std::chrono::microseconds(0) == finishTime) {
                std::cerr << "terminate() called due to shutdown() taking too long:\n" <<
                "   There was still outstanding work\n";
                terminate();
                

                return;
            }
        }
        runner.join();
        asio_context.reset();
        running = false;
        
        if (debugFlagActive<DebugFlag::contextDebug>()) 
            std::cout << "Context is no longer running\n";
    }

    void Context::terminate() {
        if (!running) {
            if (debugFlagActive<DebugFlag::contextDebug>()) 
                std::cerr << "Context::terminate() called while context isn't running\n";
            return;
        }
        
        asioContextWorkGuard.reset();
        asio_context.stop();  
        runner.join();
        asio_context.reset();
        running = false;
        
        if (debugFlagActive<DebugFlag::contextDebug>()) 
            std::cout << "Context is no longer running\n";
    }



    void Context::runContext() {
        running = true;
        if (debugFlagActive<DebugFlag::contextDebug>()) 
            std::cout << "Context is running\n";
        runner = std::thread(boost::bind(&boost::asio::io_context::run,&asio_context));

    }

    const boost::asio::io_context& Context::getAsioContext() const {
        std::cout << "Returning asio context\n";
        return asio_context;
    }

    boost::asio::io_context& Context::getAsioContext() {
        return asio_context;
    }



}