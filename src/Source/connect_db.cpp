#include "connect_db.hpp"

ConnectDB::ConnectDB() {
    try {
        // Подключение к базе данных
        connect = new pqxx::connection(db);
        if (connect->is_open()) {
            CROW_LOG_INFO << "ConnectDB: Подключено к базе данных: " << connect->dbname();
        } else {
            CROW_LOG_ERROR << "Невозможно открыть базу данных";
        }

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

ConnectDB::~ConnectDB() {
    if (connect->is_open()) {
        connect->close();
    }
    delete instance;
    delete connect;
}

ConnectDB* ConnectDB::getInstance() {
    std::lock_guard<std::mutex> lock(mutex); // Защита от многопоточного доступа
    if (instance == nullptr) {
        instance = new ConnectDB();
    }
    return instance;
}