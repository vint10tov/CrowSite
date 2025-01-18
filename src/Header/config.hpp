#pragma once

#include <crow.h>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <cstdlib> // Для exit

class Config {
    public:
        Config();
        ~Config() {}
        int get_port() const {return port;}
        const std::string & get_dir_templates() const {return dir_templates;}
        const std::string & get_dir_static_css() const {return dir_static_css;}
        const std::string & get_db() const {return db;}
        const std::string & get_sql() const {return sql;}
        const std::string & get_port_1() const {return port_1;}
        const std::string & get_port_2() const {return port_2;}
    private:
        const std::string filename = "config_crow.json";
        // порт приложения
        int port = 8000;

        // папка с шаблонами
        std::string dir_templates = "../src/templates/";
        // папка с css файлами
        std::string dir_static_css = "../src/static/css/";

        // полная строка для подключения к базе данных
        std::string db;
        // название базы данных
        std::string db_name = "?";
        // имя пользователя базой данных
        std::string db_user = "?";
        // пароль от базы данных
        std::string db_password = "?";
        
        // sql запрос
        std::string sql;
        // название таблицы пользователей
        std::string sql_name_table = "?";
        // пользователь приложения
        std::string sql_user_name = "?";
        // пароль пользователя
        std::string sql_user_password = "?";
        // солью
        std::string sql_salt = "?";
        // роль пользователя
        std::string sql_user_role = "?";

        // COM порт 1
        std::string port_1 = "/dev/ttyUSB0";
        // COM порт 2
        std::string port_2 = "/dev/ttyUSB1";

        // выгрузка данных в JSON файл
        void saveToJson() const;
        // загрузка данных из JSON файла
        bool loadFromJson();
        // формирование строки для подключения к базе данных
        void makeDb();
        // формирование sql запроса
        void makeSql();
};