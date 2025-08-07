#pragma once
#include "database.h"
#include <sqlite3.h>

#include <iostream>
#include <fstream>
#include <exception>
#include <ranges>

class SQLiteDatabase : public Database{

    public:
        SQLiteDatabase(std::string && db_filename);
        virtual ~SQLiteDatabase();

        SQLiteDatabase(const SQLiteDatabase &) = delete;
        SQLiteDatabase & operator=(const SQLiteDatabase &) = delete;

        virtual bool CreateDatabase() override;
        
        virtual bool CreateTable(std::string_view create_table_sql) override;
        
        virtual void StartDatabase() override;
        
        virtual void CloseDatabase() override;

        virtual std::vector<std::string> ImportCreateSQL(std::string_view tables_file_path) override;
        
        sqlite3 * getdb() const {return sql_db_ptr;}
        
        
    private:
        std::string sql_db_filename;
        int sql_operator_code;
        sqlite3 * sql_db_ptr;

        
};