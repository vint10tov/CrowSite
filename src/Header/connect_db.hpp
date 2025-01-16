#pragma once

#include <crow.h>
#include <pqxx/pqxx>//g++ main.cpp -o test_db -I/usr/include/postgresql -L/usr/lib/postgresql -lpqxx -lpq
#include <mutex>

#include "user.hpp"

class ConnectDB final {
    private:
        static bool object_exists;     // Существование объекта этого класса в программе
        bool status_object = false;    // Статус объекта (fals - не активен)
        static std::mutex mutex;       // Мьютекс для защиты многопоточности
        pqxx::connection * connect = nullptr;
        const char db[67] = "dbname=crowbase user=crowserwer password=885596tovbazacrowserwer@V";
        
        // Запрет на копирование экземпляров
        ConnectDB(const ConnectDB&) = delete;
        ConnectDB& operator=(const ConnectDB&) = delete;
    public:
        ConnectDB();
        ~ConnectDB();
        // получение данных пользователя
        bool get_user(std::string & username, User & user) const;
        // проверка наличия объекта
        bool get_status_object() const {return status_object;}
};