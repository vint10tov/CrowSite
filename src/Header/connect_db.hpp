#pragma once

#include <crow.h>
#include <pqxx/pqxx>//g++ main.cpp -o test_db -I/usr/include/postgresql -L/usr/lib/postgresql -lpqxx -lpq
#include <mutex>

#include "user.hpp"
#include "config.hpp"

class ConnectDB final {
    private:
        static std::mutex mutex;       // Мьютекс для защиты многопоточности
        pqxx::connection * connect = nullptr;
    public:
        ConnectDB(Config & conf);
        ~ConnectDB();
        // получение данных пользователя
        bool get_user(std::string & username, User & user) const;
        // проверка наличия объекта
};