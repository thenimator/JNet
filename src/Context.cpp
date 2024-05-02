#include "Context.hpp"
#include "defines.hpp"
#include <boost/asio/io_context.hpp>

using namespace JNet;

Context::Context() : asioContextWorkGuard(boost::asio::make_work_guard(asio_context)) {

}

Context::~Context() {
    shutDown();
}

void Context::async_run() {
    runContext();
}

void Context::shutDown() {

    asioContextWorkGuard.reset();
    asio_context.restart();
    runner.join();
}

void Context::runContext() {
    runner = std::thread(boost::bind(&boost::asio::io_context::run,&asio_context));
}

const boost::asio::io_context& Context::getAsioContext() const {
    return asio_context;
}

boost::asio::io_context& Context::getAsioContext() {
    return asio_context;
}