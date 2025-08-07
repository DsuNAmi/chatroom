#pragma once

#include <string>
#include <vector>
#include "utils.h"


class Database{
    public:
        virtual ~Database() = default;

        virtual bool CreateDatabase() = 0;

        virtual bool CreateTable(std::string_view create_table_sql) = 0;

        virtual void StartDatabase() = 0;

        virtual void CloseDatabase() = 0;

        virtual std::vector<std::string> ImportCreateSQL(std::string_view tables_file_path) = 0;

};