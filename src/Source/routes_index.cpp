#include "routes.hpp"

void setup_index_routes(crow::App<crow::CookieParser, Session>& app) {
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
}