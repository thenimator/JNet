#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <thread>
#include <memory>
#include <algorithm>
#include <iostream>


//should be changed
#define PORT 16632

#define maxMessageSize 1024

enum class BuildType {
    Client,
    Server
};