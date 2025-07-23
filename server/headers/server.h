#pragma once

#include <memory>
#include <string>

class Server{
    public:
        virtual ~Server() = default;
        virtual void Run() = 0;
        virtual void Close() = 0;
};

