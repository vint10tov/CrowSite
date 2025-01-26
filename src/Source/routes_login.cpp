#include "routes.hpp"

void setup_login_routes(crow::App<crow::CookieParser, Session>& app, ConnectDB& db) {
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
}