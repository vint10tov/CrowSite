#pragma once

#include <crow.h>//https://crowcpp.org/master/getting_started/a_simple_webpage/
#include "crow/middlewares/session.h"
#include <string>

#include "connect_db.hpp"
#include "RS485Vint.hpp"
#include "url_relay.hpp"
#include "url_login.hpp"
#include "config.hpp"
#include "price.hpp"

// Определить сеанс с типом хранилища
// В этом случае хранилище файлов на диске
using Session = crow::SessionMiddleware<crow::FileStore>;

// Статические маршруты
void setup_static_routes(crow::App<crow::CookieParser, Session>& app, Config& conf);

// Вход в систему
void setup_login_routes(crow::App<crow::CookieParser, Session>& app, ConnectDB& db);

// Главные страницы
void setup_index_routes(crow::App<crow::CookieParser, Session>& app);

// Умный дом
void setup_smart_home_routes(crow::App<crow::CookieParser, Session>& app, RS485Vint& rs485);

// Электромонтаж
void setup_electro_routes(crow::App<crow::CookieParser, Session>& app, Price& price);