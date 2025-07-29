#pragma once

#include "user.h"


class Admin : public User{
    public:
        Admin();
        Admin(std::string_view name, std::string_view password);
        virtual ~Admin();

        virtual std::string GetIdentity() const override{
            return "I am an admin";
        }

        virtual Level GetLevel() const override{
            return Level::Admin;
        }

        virtual bool Register(std::string && name, std::string && password) override;
        virtual bool Login() override;
        virtual bool Logout() override;

        virtual const std::string & GetName() const override{
            return a_name;
        }
        virtual const std::string & GetId() const override{
            return a_id;
        }

    private:
        std::string a_name;
        std::string a_password;
        std::string a_id;
        bool a_new_user_flag;
        std::string a_key;


        static int a_currentAdminCount;
        

};