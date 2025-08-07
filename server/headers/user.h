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
        }

        virtual std::string GetIdentity() const = 0;
        virtual Level GetLevel() const = 0;

        virtual bool Register(std::string && name, std::string && password) = 0;
        virtual bool Login() = 0;
        virtual bool Logout() = 0;

        virtual const std::string & GetName() const = 0;
        virtual const std::string & GetId() const = 0;

        
};