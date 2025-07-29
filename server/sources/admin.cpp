#include "admin.h"

int Admin::a_currentAdminCount = 0;


Admin::Admin()
: a_name(""), a_password(""),a_id(""),
  a_new_user_flag(true),
  a_key("admin")
{
    ++a_currentAdminCount;
}

Admin::Admin(std::string_view name, std::string_view password)
: a_name(name.data()),a_password(password.data()),
 a_new_user_flag(false),
 a_key("admin")
{
    Utils::EncryptYourString(a_password, a_key);
    a_id = std::to_string(a_currentAdminCount) + a_name + ".";
}

Admin::~Admin(){
    if(a_currentAdminCount > 0){
        --a_currentAdminCount;
    }
}

bool Admin::Register(std::string && name, std::string && password){
    // name and password is null
    if(a_name == "" && a_password == ""){
        //connect to the database
        return true;
    }else{
        return false;
    }
}

bool Admin::Login(){
    //change the page
    return true;
}

bool Admin::Logout(){
    //change the page
    //save the time
    return true;
}

