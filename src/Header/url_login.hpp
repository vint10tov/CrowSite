#pragma once

#include <crow.h>
#include <string>
#include <map>
#include <openssl/sha.h> // g++ main.cpp -o my_program -lssl -lcrypto

class URLLogin {
    public:
        URLLogin();
        URLLogin(std::string body);
        bool get_login() {return check_login;}
        std::string get_username() {return username;}
    private:
        bool check_login = false;
        std::string username = "vint";
        std::string psw = "123";
        std::string salt = "82Kw2yrJE8qyLoBK";
        std::string hash = "73d125a86ba7d0cd638e8f8b837dd1b8c09061daa3bffcf471a1902a6f278d05";
        // тело запроса в ключ-значение
        std::map<std::string, std::string> parseKeyValueString(const std::string& input);
        // генератор соли
        std::string generate_salt(std::size_t length);
        // шифрование пароля
        std::string hash_password(const std::string &password, const std::string &salt);
        // поиск sql инъекций
        bool containsSqlInjection(const std::string& input);
};