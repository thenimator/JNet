#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
//#include <boost/thread.hpp>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <concepts>
#include <functional>
#include <fmt/format.h>
#include <fmt/color.h>
#include "debug/debugActive.hpp"

#define PORT 16632

#define maxMessageSize 1024

enum class BuildType {
    Client,
    Server
};