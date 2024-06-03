#include "Context.hpp"
#include "defines.hpp"
#include <boost/asio/io_context.hpp>
#include <chrono>

using namespace JNet;

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

void Context::shutDown(std::chrono::milliseconds delay, std::chrono::milliseconds finishTime) {
    if (!running) {
        if (debugFlagActive<DebugFlag::contextDebug>()) 
            std::cerr << "Context::shutDown() called while context isn't running\n";
        return;
    }

    std::this_thread::sleep_for(delay);
    
    asioContextWorkGuard.reset();
    while (!asio_context.stopped())  {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        finishTime -= std::chrono::milliseconds(1);
        if (std::chrono::milliseconds(0) == finishTime) {
            terminate();
            std::cerr << "terminate() called due to shutdown() taking too long:\n" <<
            "   There was still outstanding work\n”";

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
    return asio_context;
}

boost::asio::io_context& Context::getAsioContext() {
    return asio_context;
}