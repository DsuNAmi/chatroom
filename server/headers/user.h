#pragma once

#include <string>
#include <optional>

#include "sql_database.h"

using namespace std::literals;


enum class Level{
    Admin = 1,
    Normal,
    Guest
};


class User{
    public:
        virtual ~User() = default;
        static std::optional<Level> CheckAccount(Database * databse, 
            std::string && user_id,
        std::string && password){
            //check the account;
            if(!databse->Isin("user"sv,Utils::ConstructQuery({"name"sv,"password"sv},
            std::make_tuple(user_id, password)))){
                throw std::invalid_argument("Your user_id or password is wrong.\n");
            }
            std::string query_sql = std::format("select type from user where name={} and password={}",
            user_id,password);
            std::string type = databse->Query(query_sql);
            if(type == "admin"){
                return std::optional<Level>(Level::Admin);
            }
            else if(type == "guest"){
                return std::optional<Level>(Level::Guest);
            }
            return std::nullopt;
        }
        virtual std::string GetIdentity() const = 0;
    protected:
        // virtual Level GetLevel() = 0;

        // virtual bool Register() = 0;
        // virtual bool Login() = 0;
        // virtual bool Logout() = 0;
};