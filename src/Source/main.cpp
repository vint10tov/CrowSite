#include <crow.h>//https://crowcpp.org/master/getting_started/a_simple_webpage/
#include <string>

#include "url_relay.hpp"
#include "connect_db.hpp"
#include "url_login.hpp"

const std::string dir_templates = "../src/templates/";
const std::string dir_static_css = "../src/static/css/";

// Определение статических переменных
Uart* Uart::instance = nullptr;
std::mutex Uart::mutex;
ConnectDB* ConnectDB::instance = nullptr;
std::mutex ConnectDB::mutex;

int main()
{
    crow::SimpleApp app;

    crow::mustache::set_global_base(dir_templates);

    Uart      * uart = Uart::getInstance();
    ConnectDB * db   = ConnectDB::getInstance();

    //Статический маршрут css
    CROW_ROUTE(app, "/static/css/<path>")
    ([](const std::string& filepath) {
        crow::response res;
        res.set_static_file_info_unsafe(dir_static_css + filepath);  // Указать путь к файлу
        return res;
    });

    // Определяем маршрут для страницы логирования
    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)(
        [](const crow::request &req){

        auto page = crow::mustache::load("login.html");

        if (req.method == crow::HTTPMethod::GET) {

        } else if (req.method == crow::HTTPMethod::POST) {
            URLLogin u_login(req.body);
        }
        // Создаем контекст (JSON-объект)
        crow::mustache::context ctx;
        ctx["title"] = "Вход в систему";

        return page.render(ctx);
    });

    // Определяем маршрут для главной страницы
    CROW_ROUTE(app, "/")([](){
        auto page = crow::mustache::load("index.html");

        // Создаем контекст (JSON-объект)
        crow::mustache::context ctx;
        ctx["title"] = "Главная страница";

        return page.render(ctx);
    });

    // Определяем маршрут для страницы управления реле
    CROW_ROUTE(app, "/relay").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)(
        [uart](const crow::request &req){

        auto page = crow::mustache::load("relay.html");
        URLRelay::StringsForTemplate sft;

        if (req.method == crow::HTTPMethod::GET) {

            URLRelay u_relay(uart);
            sft = u_relay.GET_StringsForTemplate();

        } else if (req.method == crow::HTTPMethod::POST) {

            URLRelay u_relay(uart, req.body);
            sft = u_relay.GET_StringsForTemplate();
        }

        // Создаем контекст (JSON-объект)
        crow::mustache::context ctx;
        ctx["title"]       = sft.title;
        ctx["error"]       = sft.error;
        ctx["time"]        = sft.time;
        ctx["timeR2M0ON"]  = sft.timeR2M0ON;
        ctx["timeR2M0OFF"] = sft.timeR2M0OFF;
        ctx["timeR2M1ON"]  = sft.timeR2M1ON;
        ctx["timeR2M1OFF"] = sft.timeR2M1OFF;
        ctx["statusR0"]    = sft.statusR0;
        ctx["RstatusR0"]   = sft.RstatusR0;
        ctx["statusR1"]    = sft.statusR1;
        ctx["RstatusR1"]   = sft.RstatusR1;
        ctx["statusR2"]    = sft.statusR2;
        ctx["RstatusR2"]   = sft.RstatusR2;
        ctx["statusR2M0"]  = sft.statusR2M0;
        ctx["RstatusR2M0"] = sft.RstatusR2M0;
        ctx["statusR2M1"]  = sft.statusR2M1;
        ctx["RstatusR2M1"] = sft.RstatusR2M1;

        
        // Генерация страницы с использованием Mustache шаблона
        return page.render(ctx);
    });

    // Запускаем сервер на порту 
    app.port(8000)
    .multithreaded()
    .run();
}