#pragma once
#include "database.h"
#include <sqlite3.h>

#include <iostream>

class SQLiteDatabase : public Database{

    public:
        SQLiteDatabase() = default;
        virtual ~SQLiteDatabase() = default;

        virtual bool Isin(std::string_view dbname, 
            Utils::queryArgs && args) override;

        virtual std::string Query(std::string_view query_sql) override;

    private:
        std::string ConstructWhere(Utils::queryArgs && args);
        
        
};