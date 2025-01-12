#pragma once

#include <crow.h>
#include <pqxx/pqxx>//g++ main.cpp -o test_db -I/usr/include/postgresql -L/usr/lib/postgresql -lpqxx -lpq
#include <mutex>

class ConnectDB final {
    private:
        static ConnectDB* instance;    // Указатель на единственный экземпляр
        static std::mutex mutex;       // Мьютекс для защиты многопоточности

        pqxx::connection * connect = nullptr;
      
        const char db[67] = "dbname=crowbase user=crowserwer password=885596tovbazacrowserwer@V";
        // Закрытый конструктор для предотвращения создания экземпляров
        ConnectDB();
        // Закрытый деструктор
        ~ConnectDB();

        // Запрет на копирование экземпляров
        ConnectDB(const ConnectDB&) = delete;
        ConnectDB& operator=(const ConnectDB&) = delete;
    public:
        // Метод для получения единственного экземпляра класса
        static ConnectDB* getInstance();
};