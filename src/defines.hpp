#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>



#define PORT 16632

#define maxMessageSize 1024

enum class BuildType {
    Client,
    Server
};