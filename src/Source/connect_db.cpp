#include "connect_db.hpp"

bool ConnectDB::object_exists = false;
std::mutex ConnectDB::mutex;

ConnectDB::ConnectDB() {
    if (object_exists) {
        return;
    }
    object_exists = true;
    try {
        // Подключение к базе данных
        connect = new pqxx::connection(db);
        if (connect->is_open()) {
            CROW_LOG_INFO << "ConnectDB: Подключено к базе данных: " << connect->dbname();
            status_object = true;
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
    object_exists = false;
    delete connect;
}