#include "routes.hpp"

void setup_static_routes(crow::App<crow::CookieParser, Session>& app, Config& conf) {

    // favicon.ico
    CROW_ROUTE(app, "/favicon.ico").methods(crow::HTTPMethod::GET)
    ([]() {
        crow::response res;
        res.set_static_file_info_unsafe("../src/static/favicon/favicon.ico");  // Указать путь к файлу
        return res;
    });

    // css
    CROW_ROUTE(app, "/static/css/<path>").methods(crow::HTTPMethod::GET)
    ([&conf](const std::string& filepath) {
        crow::response res;
        res.set_static_file_info_unsafe(conf.get_dir_static_css() + filepath);  // Указать путь к файлу
        return res;
    });

    // Видеонаблюдение
    CROW_ROUTE(app, "/video").methods(crow::HTTPMethod::GET)
    ([](const crow::request& req) {
        crow::response rsp;
        rsp.code = 302;
        rsp.add_header("Location", "http://127.0.0.1:8765");
        return rsp;
    });
}