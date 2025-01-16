#pragma once 

#include <string>

struct User {
    std::string id;
    std::string username;
    std::string userrole;
    std::string psw;
    std::string salt;
};