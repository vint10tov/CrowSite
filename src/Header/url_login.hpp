#pragma once

#include <crow.h>
#include <string>
#include <map>
#include <openssl/sha.h> // g++ main.cpp -o my_program -lssl -lcrypto

#include "connect_db.hpp"
#include "user.hpp"

class URLLogin {
    public:
        URLLogin(std::string body, ConnectDB & db);
        bool get_login() {return check_login;}
        std::string get_username() {return user.username;}
        std::string get_userrole() {return user.userrole;}
    private:
        bool check_login = false;
        User user;
        // тело запроса в ключ-значение
        std::map<std::string, std::string> parseKeyValueString(const std::string& input);
        // генератор соли
        std::string generate_salt(std::size_t length);
        // шифрование пароля
        std::string hash_password(const std::string &password, const std::string &salt);
        // поиск sql инъекций
        bool containsSqlInjection(const std::string& input);
};