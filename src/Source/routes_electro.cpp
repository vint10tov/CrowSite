#include "routes.hpp"

void setup_electro_routes(crow::App<crow::CookieParser, Session>& app, Price& price) {
    // Определяем маршрут для страницы цен
    CROW_ROUTE(app, "/prices").methods(crow::HTTPMethod::GET)(
    [&price](const crow::request& req){

        auto page = crow::mustache::load("prices.html");

        // Создаем контекст (JSON-объект)
        crow::mustache::context ctx;
        ctx["title"] = "Электромонтаж";

        crow::mustache::context::list servicesList; // Создаем список услуг
        // Проходим по услугам и заполняем json::wvalue
        for (const auto& service : price.get_services()) {
            crow::mustache::context::object serviceObj; // Создаем объект для каждой услуги
            serviceObj["id"] = service.id;                    // Устанавливаем ID
            serviceObj["name"] = service.name;                // Устанавливаем название
            serviceObj["unit_of_measurement"] = service.unit_of_measurement; // Устанавливаем единицу измерения
            serviceObj["price"] = service.price;              // Устанавливаем цену
            servicesList.push_back(std::move(serviceObj));  // Добавляем объект услуги в список
        }
        ctx["prices"] = std::move(servicesList); // Добавляем список услуг в контекст

        return crow::response{page.render(ctx)};
    });
}