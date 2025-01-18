#include "config.hpp"

Config::Config() {
    if (loadFromJson()) {
        makeDb();
        makeSql();
    } else {
        CROW_LOG_WARNING << "Config: Не удалось загрузить конфигурацию из файла";
        saveToJson();
        //exit(EXIT_FAILURE); // Завершение программы с кодом ошибки
        throw std::runtime_error("Config: Не удалось загрузить конфигурацию"); // Выбрасываем исключение
    }
}

// Метод для выгрузки данных в JSON файл
void Config::saveToJson() const {
    nlohmann::json j;
    j["port"] = port;
    j["dir_templates"]     = dir_templates;
    j["dir_static_css"]    = dir_static_css;
    j["db_name"]           = db_name;
    j["db_user"]           = db_user;
    j["db_password"]       = db_password;
    j["sql_name_table"]    = sql_name_table;
    j["sql_user_name"]     = sql_user_name;
    j["sql_user_password"] = sql_user_password;
    j["sql_salt"]          = sql_salt;
    j["sql_user_role"]     = sql_user_role;
    j["port_1"]            = port_1;
    j["port_2"]            = port_2;

    std::ofstream fs("example_" + filename);
    if (fs.is_open()) {
        fs << j.dump(4); // 4 - отступы для форматирования
        fs.close();
    } else {
        CROW_LOG_WARNING << "Config: Не удалось открыть файл для сохранения конфигурации";
    }
}

// Метод для загрузки данных из JSON файла
bool Config::loadFromJson() {
    nlohmann::json j;
    std::ifstream fs(filename);
    if (fs.is_open()) {
        fs >> j;
        fs.close();

        port              = j["port"];
        dir_templates     = j["dir_templates"];
        dir_static_css    = j["dir_static_css"];
        db_name           = j["db_name"];
        db_user           = j["db_user"];
        db_password       = j["db_password"];
        sql_name_table    = j["sql_name_table"];
        sql_user_name     = j["sql_user_name"];
        sql_user_password = j["sql_user_password"];
        sql_salt          = j["sql_salt"];
        sql_user_role     = j["sql_user_role"];
        port_1            = j["port_1"];
        port_2            = j["port_2"];
        return true;
    } else {
        CROW_LOG_ERROR << "Config: Не удалось открыть файл для загрузки конфигурации";
        return false;
    }
}

void Config::makeDb() {
    db = "dbname=";
    db += db_name;
    db += " user=";
    db += db_user;
    db += " password=";
    db += db_password;
}

void Config::makeSql() {
    sql = "INSERT INTO ";
    sql += sql_name_table;
    sql += " (name, password, salt, role)\n";
    sql += "VALUES ('";
    sql += sql_user_name;
    sql += "', '";
    sql += sql_user_password;
    sql += "', '";
    sql += sql_salt;
    sql += "', '";
    sql += sql_user_role;
    sql += "')\n";
    sql += R"(ON CONFLICT (name) DO UPDATE SET 
        password = EXCLUDED.password,
        salt = EXCLUDED.salt,
        role = EXCLUDED.role;
    )";
}