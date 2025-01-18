#include "connect_db.hpp"

bool ConnectDB::object_exists = false;
std::mutex ConnectDB::mutex;

ConnectDB::ConnectDB(Config & conf) {
    if (object_exists) {
        return;
    }
    object_exists = true;
    try {
        // Подключение к базе данных
        connect = new pqxx::connection(conf.get_db());
        if (connect->is_open()) {
            CROW_LOG_INFO << "ConnectDB: Подключено к базе данных: " << connect->dbname();

            // Создание объекта транзакции
            pqxx::work W(*connect);

            // SQL-запрос для создания таблицы, если она не существует
            std::string create_table_sql = R"(
                CREATE TABLE IF NOT EXISTS users (
                    id SERIAL PRIMARY KEY,
                    name VARCHAR(100) NOT NULL,
                    password VARCHAR(255) NOT NULL,
                    salt VARCHAR(50) NOT NULL,
                    role VARCHAR(50) NOT NULL,
                    CONSTRAINT unique_name UNIQUE (name)
                );
            )";

            // Выполнение запроса на создание таблицы
            W.exec(create_table_sql);

            // SQL-запрос для вставки записей о пользователях
            std::string insert_user_sql = conf.get_sql();

            // Выполнение запроса на вставку записи
            W.exec(insert_user_sql);

            // Завершение транзакции
            W.commit();

            status_object = true;
        } else {
            CROW_LOG_ERROR << "ConnectDB: Невозможно открыть базу данных";
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

bool ConnectDB::get_user(std::string & username, User & user) const {
    // Создание объекта транзакции
    pqxx::work W(*connect);

    // SQL-запрос для извлечения данных пользователя по имени
    std::string sql_query = "SELECT id, name, password, salt, role FROM users WHERE name = " +
                            W.quote(username) + ";";
    
    // Выполнение запроса
    pqxx::result R = W.exec(sql_query);

    // Проверка наличия результатов
    if (!R.empty()) {
        // Получение данных из результата
        user.id       = R[0][0].as<std::string>(); // id
        user.username = R[0][1].as<std::string>(); // name
        user.psw      = R[0][2].as<std::string>(); // password
        user.salt     = R[0][3].as<std::string>(); // salt
        user.userrole = R[0][4].as<std::string>(); // role
        // Завершение транзакции
        W.commit();
        return true;
    } else {
        // Завершение транзакции
        W.commit();
        return false;
    }
}