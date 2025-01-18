#include <crow.h>//https://crowcpp.org/master/getting_started/a_simple_webpage/
#include "crow/middlewares/session.h"
#include <string>

#include "connect_db.hpp"
#include "RS485Vint.hpp"
#include "url_relay.hpp"
#include "url_login.hpp"
#include "config.hpp"

int main() {

    Config conf;
    ConnectDB db(conf);
    RS485Vint rs485(conf);

    crow::mustache::set_global_base(conf.get_dir_templates());

    // определить сеанс с типом хранилища
    // В этом случае хранилище файлов на диске
    using Session = crow::SessionMiddleware<crow::FileStore>;

    crow::App<crow::CookieParser, Session> app {Session{
        crow::FileStore{"./sessions"} // убедитесь, что эта папка существует!!
    }};

    //Статический маршрут css
    CROW_ROUTE(app, "/static/css/<path>")
    ([&conf](const std::string& filepath) {
        crow::response res;
        res.set_static_file_info_unsafe(conf.get_dir_static_css() + filepath);  // Указать путь к файлу
        return res;
    });

    // Определяем маршрут для страницы логирования
    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::GET)(
        [&app](const crow::request &req){

        auto page = crow::mustache::load("login.html");
        auto& session = app.get_context<Session>(req);
        std::string out = "";

        // показать «карточку» с сообщением, если необходимо
        auto flash = session.get("flash", "");
        if (!flash.empty()) out += flash;
        // удалить сообщение из сеанса, чтобы не показывать его во второй раз
        session.remove("flash");

        // Забанить пользователя, если он пытался часто
        // Второй параметр — это резерв
        // — что вернуть, если запись не найдена или имеет другой тип
        // session.contains() просто проверяет наличие независимо от типа
        if (session.get("tries", 0) > 3) {
            out += "Вы часто пытались!";
        }

        // Создаем контекст (JSON-объект)
        crow::mustache::context ctx;
        ctx["title"] = "Вход в систему";
        ctx["out"] = out;

        return page.render(ctx);
    });

    // Обработчик входа
    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)(
        [&app, &db](const crow::request& req) {
        
        auto& session = app.get_context<Session>(req);
        session.apply("tries", [](int v) {return v + 1; });

        URLLogin u_login(req.body, db);
        std::string username = u_login.get_username();
        std::string userrole = u_login.get_userrole();

        crow::response rsp;
        rsp.code = 303;
        if (u_login.get_login()) {
            // просто сохраните, что пользователь аутентифицирован
            // это безопасно и просто
            session.set("user", username);
            session.set("role", userrole);
            rsp.add_header("Location", "/");
        } else {
            session.set("flash", "Неправильный пароль");
            rsp.add_header("Location", "/login");
        }
        return rsp;
    });

    // Определяем маршрут для главной страницы
    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)(
        [&app](const crow::request& req) -> crow::response {

        auto& session = app.get_context<Session>(req);
        auto page = crow::mustache::load("index.html");
        auto username = session.get("user", "");

        // Проверка, если пользователь не авторизован
        if (username.empty()) {
            // Создаем ответ с редиректом на страницу входа
            crow::response res(302);
            res.set_header("Location", "/login"); // Указываем заголовок для перенаправления
            return res; // Возвращаем ответ с редиректом
        }

        // Создаем контекст (JSON-объект)
        crow::mustache::context ctx;
        ctx["title"] = "Главная страница";

        // Рендерим шаблон с контекстом
        return crow::response{page.render(ctx)}; // Оборачиваем отрисованный шаблон в crow::response
    });

    // Определяем маршрут для страницы управления реле
    CROW_ROUTE(app, "/relay").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)(
        [&app, &rs485](const crow::request &req){

        auto& session = app.get_context<Session>(req);
        auto page = crow::mustache::load("relay.html");
        auto username = session.get("user", "");
        URLRelay::StringsForTemplate sft;

        // Проверка, если пользователь не авторизован
        if (username.empty()) {
            // Создаем ответ с редиректом на страницу входа
            crow::response res(302);
            res.set_header("Location", "/login"); // Указываем заголовок для перенаправления
            return res; // Возвращаем ответ с редиректом
        }

        if (req.method == crow::HTTPMethod::GET) {

            URLRelay u_relay(rs485);
            sft = u_relay.GET_StringsForTemplate();

        } else if (req.method == crow::HTTPMethod::POST) {

            URLRelay u_relay(rs485, req.body);
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

        // Рендерим шаблон с контекстом
        return crow::response{page.render(ctx)}; // Оборачиваем отрисованный шаблон в crow::response
    });

    // Запускаем сервер на порту 
    app.port(conf.get_port())
    .multithreaded()
    .run();
}