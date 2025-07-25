#pragma once

#include "user.h"


class Admin : public User{
    public:
        Admin() = default;
        virtual ~Admin() = default;

        virtual std::string GetIdentity() const override{
            return "I am an admin";
        }

    private:
};