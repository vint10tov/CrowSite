#include "routes.hpp"

void setup_smart_home_routes(crow::App<crow::CookieParser, Session>& app, RS485Vint& rs485) {
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
}