#pragma once

class Database{
    public:
        virtual bool Connection() = 0;
        virtual ~Database() = default;
};