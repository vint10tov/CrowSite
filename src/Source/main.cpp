#include <crow.h>
#include <unordered_map>
#include <sstream>
#include <string>

//https://crowcpp.org/master/getting_started/a_simple_webpage/

bool isInteger(const std::string& str);
std::unordered_map<std::string, int> parseKeyValueString(const std::string& input);

int main()
{
    crow::SimpleApp app;

    // Статический маршрут для конкретного файла
    CROW_ROUTE(app, "/css/<string>")
    ([](const std::string& filepath) {
        crow::response res;
        res.set_static_file_info("static/" + filepath);  // Указать путь к файлу
        return res;
    });

    // Определяем маршрут для главной страницы
    CROW_ROUTE(app, "/")([](){
        auto page = crow::mustache::load("index.html");

        // Создаем контекст (JSON-объект)
        crow::mustache::context ctx;
        ctx["title"] = "Главная страница";

        return page.render(ctx);
    });

    // Определяем маршрут для главной страницы
    CROW_ROUTE(app, "/relay").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)(
        [](const crow::request &req){
        auto page = crow::mustache::load("relay.html");

        // Создаем контекст (JSON-объект)
        crow::mustache::context ctx;
        ctx["title"] = "Управление Реле";
        ctx["error"] = "error ????????????????????";
        ctx["time"] = "00:00:00/0/0/2000";
        ctx["timeR2M0ON"] = "00:00:00";
        ctx["timeR2M0OFF"] = "00:00:00";
        ctx["timeR2M1ON"] = "00:00:00";
        ctx["timeR2M1OFF"] = "00:00:00";
        ctx["statusR0"] = "ON";
        ctx["RstatusR0"] = "1";
        ctx["statusR1"] = "ON";
        ctx["RstatusR1"] = "1";
        ctx["statusR2"] = "ON";
        ctx["RstatusR2"] = "1";
        ctx["statusR2M0"] = "ACT";
        ctx["RstatusR2M0"] = "1";
        ctx["statusR2M1"] = "ACT";
        ctx["RstatusR2M1"] = "1";

        if (req.method == crow::HTTPMethod::POST) {
            auto result = parseKeyValueString(req.body);
            // Вывод результата
            for (const auto& pair : result) {
                std::cout << pair.first << " = " << pair.second << std::endl;
            }
        }

        return page.render(ctx);
    });

    // Запускаем сервер на порту 
    app.port(8000)
    .multithreaded()
    .run();
}

bool isInteger(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

std::unordered_map<std::string, int> parseKeyValueString(const std::string& input) {
    std::unordered_map<std::string, int> result;
    std::istringstream stream(input);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        std::string key;
        std::string value;

        size_t equalsPos = pair.find('=');
        if (equalsPos != std::string::npos) {
            key = pair.substr(0, equalsPos);
            value = pair.substr(equalsPos + 1);
        } else {
            key = pair;
            value = "";
        }

        if (isInteger(value)) {
            result[key] = std::stoi(value);
        } else {
            result[key] = 0;
        }
    }

    return result;
}