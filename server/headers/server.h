#pragma once

#include <memory>
#include <string>

class Server{
    public:
        virtual ~Server() = default;
        virtual void Run(std::string_view ip_addr, int port_num) = 0;
        virtual void Close() = 0;
};

