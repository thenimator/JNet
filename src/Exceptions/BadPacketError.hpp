#pragma once
#include <exception>
#include <stdexcept>




namespace JNet {
    class BadPacketError : public std::runtime_error {
    public:
        BadPacketError( const std::string& what_arg ) : std::runtime_error(what_arg) {};
        BadPacketError( const char* what_arg ) : std::runtime_error(what_arg) {};
        BadPacketError( const BadPacketError& other ) : std::runtime_error(other) {};
    };
} 

