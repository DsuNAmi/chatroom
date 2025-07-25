#include "sql_database.h"



bool SQLiteDatabase::Isin(std::string_view dbname, Utils::queryArgs && args){
    std::string sql = std::format("select count(1) from {} {}",dbname.data(),
    ConstructWhere(std::move(args)));
    std::cout << sql << std::endl;
    return true;
}


std::string SQLiteDatabase::ConstructWhere(Utils::queryArgs && args){
    std::string where = "where ";
    for(const auto & [k, v] : args){
        where += std::string(std::format("{}={}",k,v));
        where += " and ";
    }
    where.replace(where.rfind(" and "), 5, ";");

    return where;
}


std::string SQLiteDatabase::Query(std::string_view query_sql){
    return "guest";
}


