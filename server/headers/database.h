#pragma once

#include <string>
#include "utils.h"


class Database{
    public:
        virtual ~Database() = default;

        virtual std::string Query(std::string_view query_sql) = 0;


        //query
        virtual bool Isin(std::string_view dbname, 
            Utils::queryArgs && args) = 0;


};