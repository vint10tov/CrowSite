#include <crow.h>//https://crowcpp.org/master/getting_started/a_simple_webpage/
#include "crow/middlewares/session.h"

#include "routes.hpp"
#include "config.hpp"
#include "connect_db.hpp"
#include "RS485Vint.hpp"
#include "price.hpp"

int main() {

    Config conf;
    ConnectDB db(conf);
    RS485Vint rs485(conf);
    Price price("price.xls");

    crow::mustache::set_global_base(conf.get_dir_templates());

    crow::App<crow::CookieParser, Session> app {Session{
        crow::FileStore{"./sessions"} // убедитесь, что эта папка существует!!
    }};

    // Статические маршруты
    setup_static_routes(app, conf);
    // Вход в систему
    setup_login_routes(app, db);
    // Главные страницы
    setup_index_routes(app);
    // Умный дом
    setup_smart_home_routes(app, rs485);
    // Электромонтаж
    setup_electro_routes(app, price);
    
    // Запускаем сервер
    app.port(conf.get_port())
    .multithreaded()
    .run();
}