#pragma once

#include <crow.h>
#include <string>
#include <map>
#include <openssl/sha.h> // g++ main.cpp -o my_program -lssl -lcrypto

class URLLogin {
    public:
        URLLogin();
        URLLogin(std::string body);
    private:
        // тело запроса в ключ-значение
        std::map<std::string, std::string> parseKeyValueString(const std::string& input);
        // генератор соли
        std::string generate_salt(std::size_t length);
        // шифрование пароля
        std::string hash_password(const std::string &password, const std::string &salt);
        // поиск sql инъекций
        bool containsSqlInjection(const std::string& input);
};