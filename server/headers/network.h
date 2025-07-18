#pragma once


class Network{
    public:
        virtual void Perpare() = 0;
        virtual bool Connection() = 0;

        virtual ~Network() = default;
};

