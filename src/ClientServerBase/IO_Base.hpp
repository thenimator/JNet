#pragma once
#include "../serverbase.hpp"

namespace JNet {
    template<bool TIncludeEndpoint>
    class IO_BASE {

    };

    template<>
    class IO_BASE<true> : public ServerBase {
    public:
        IO_BASE(uint16_t port) : ServerBase(port) {};
        IO_BASE(char err) : ServerBase(err) {};
    };
}