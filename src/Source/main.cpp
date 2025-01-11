#include <crow.h>//https://crowcpp.org/master/getting_started/a_simple_webpage/
#include <map>
#include <sstream>
#include <string>
#include <ctime>
#include <chrono>

#include "uart.hpp"
#include "request_uart.hpp"
#include "smart_home.hpp"

bool isInteger(const std::string& str);
std::map<std::string, uint8_t> parseKeyValueString(const std::string& input);
RequestUATR map_to_RequestUATR(const std::map<std::string, uint8_t>& map);
void time_server(uint & hour, uint & minute);

// Определение статических переменных
Uart* Uart::instance = nullptr;
std::mutex Uart::mutex;

int main()
{
    crow::SimpleApp app;

    Uart * uart = Uart::getInstance();

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

    // Определяем маршрут для страницы управления реле
    CROW_ROUTE(app, "/relay").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)(
        [uart](const crow::request &req){

        auto page = crow::mustache::load("relay.html");

        SmartHome s_home(0);
        static const int SIZE_BUF_in  = 21;
        static const int SIZE_BUF_out = 11;
        uint8_t bufer_in[SIZE_BUF_in];
        uint8_t bufer_out[SIZE_BUF_out];

        // Создаем строки для шаблонизатора
        std::string title       = "Управление Реле";
        std::string error       = "ERROR";
        std::string time        = "00:00:00/0/0/2000";
        std::string timeR2M0ON  = "00:00:00";
        std::string timeR2M0OFF = "00:00:00";
        std::string timeR2M1ON  = "00:00:00";
        std::string timeR2M1OFF = "00:00:00";
        std::string statusR0    = "ON";
        std::string RstatusR0   = "1";
        std::string statusR1    = "ON";
        std::string RstatusR1   = "1";
        std::string statusR2    = "ON";
        std::string RstatusR2   = "1";
        std::string statusR2M0  = "ACT";
        std::string RstatusR2M0 = "1";
        std::string statusR2M1  = "ACT";
        std::string RstatusR2M1 = "1";

        if (req.method == crow::HTTPMethod::GET) {
            // объект запроса для платы
            RequestUATR r_uart;
            if (r_uart.serialize(bufer_out, SIZE_BUF_out)) {
                if (uart->sending_string(bufer_in, bufer_out, SIZE_BUF_in, SIZE_BUF_out)) {
                    if(s_home.deserialize(bufer_in, SIZE_BUF_in)) {
                        error = "OK";
                        time = s_home.showTime();
                        timeR2M0ON = s_home.showTimeModOn(2, 0);
                        timeR2M0OFF = s_home.showTimeModOff(2, 0);
                        timeR2M1ON = s_home.showTimeModOn(2, 1);
                        timeR2M1OFF = s_home.showTimeModOff(2, 1);
                        if (s_home.GET_status_relay(0)) {
                            statusR0 = "OFF";
                            RstatusR0 = "0";
                        } else {
                            statusR0 = "ON";
                            RstatusR0 = "1";
                        }
                        if (s_home.GET_status_relay(1)) {
                            statusR1 = "OFF";
                            RstatusR1 = "0";
                        } else {
                            statusR1 = "ON";
                            RstatusR1 = "1";
                        }
                        if (s_home.GET_status_relay(2)) {
                            statusR2 = "OFF";
                            RstatusR2 = "0";
                        } else {
                            statusR2 = "ON";
                            RstatusR2 = "1";
                        }
                        if (s_home.GET_status_mod(2, 0)) {
                            statusR2M0 = "DEACT";
                            RstatusR2M0 = "0";
                        } else {
                            statusR2M0 = "ACT";
                            RstatusR2M0 = "1";
                        }
                        if (s_home.GET_status_mod(2, 1)) {
                            statusR2M1 = "DEACT";
                            RstatusR2M1 = "0";
                        } else {
                            statusR2M1 = "ACT";
                            RstatusR2M1 = "1";
                        }
                    } else {
                        error = "ERROR: ошибка десериализации SmartHome";
                    }
                } else {
                    error = "ERROR: ошибка отправки или принятия данных";
                }
            } else {
                error = "ERROR: ошибка сериализации RequestUATR";
            }

        } else if (req.method == crow::HTTPMethod::POST) {

            // парсинг ключ-значение из тела запроса
            auto result = parseKeyValueString(req.body);

            // Вывод результата
            for (const auto& pair : result) {
                CROW_LOG_INFO << "Тело запроса: " << pair.first << " = " << (int)pair.second;
            }

            // объект запроса для платы
            RequestUATR r_uart = map_to_RequestUATR(result);
            if (r_uart.serialize(bufer_out, SIZE_BUF_out)) {
                if (uart->sending_string(bufer_in, bufer_out, SIZE_BUF_in, SIZE_BUF_out)) {
                    if(s_home.deserialize(bufer_in, SIZE_BUF_in)) {
                        error = "OK";
                        time = s_home.showTime();
                        timeR2M0ON = s_home.showTimeModOn(2, 0);
                        timeR2M0OFF = s_home.showTimeModOff(2, 0);
                        timeR2M1ON = s_home.showTimeModOn(2, 1);
                        timeR2M1OFF = s_home.showTimeModOff(2, 1);
                        if (s_home.GET_status_relay(0)) {
                            statusR0 = "OFF";
                            RstatusR0 = "0";
                        } else {
                            statusR0 = "ON";
                            RstatusR0 = "1";
                        }
                        if (s_home.GET_status_relay(1)) {
                            statusR1 = "OFF";
                            RstatusR1 = "0";
                        } else {
                            statusR1 = "ON";
                            RstatusR1 = "1";
                        }
                        if (s_home.GET_status_relay(2)) {
                            statusR2 = "OFF";
                            RstatusR2 = "0";
                        } else {
                            statusR2 = "ON";
                            RstatusR2 = "1";
                        }
                        if (s_home.GET_status_mod(2, 0)) {
                            statusR2M0 = "DEACT";
                            RstatusR2M0 = "0";
                        } else {
                            statusR2M0 = "ACT";
                            RstatusR2M0 = "1";
                        }
                        if (s_home.GET_status_mod(2, 1)) {
                            statusR2M1 = "DEACT";
                            RstatusR2M1 = "0";
                        } else {
                            statusR2M1 = "ACT";
                            RstatusR2M1 = "1";
                        }
                    } else {
                        error = "ERROR: ошибка десериализации SmartHome";
                    }
                } else {
                    error = "ERROR: ошибка отправки или принятия данных";
                }
            } else {
                error = "ERROR: ошибка сериализации RequestUATR";
            }
        }

        // Создаем контекст (JSON-объект)
        crow::mustache::context ctx;
        ctx["title"]       = title;
        ctx["error"]       = error;
        ctx["time"]        = time;
        ctx["timeR2M0ON"]  = timeR2M0ON;
        ctx["timeR2M0OFF"] = timeR2M0OFF;
        ctx["timeR2M1ON"]  = timeR2M1ON;
        ctx["timeR2M1OFF"] = timeR2M1OFF;
        ctx["statusR0"]    = statusR0;
        ctx["RstatusR0"]   = RstatusR0;
        ctx["statusR1"]    = statusR1;
        ctx["RstatusR1"]   = RstatusR1;
        ctx["statusR2"]    = statusR2;
        ctx["RstatusR2"]   = RstatusR2;
        ctx["statusR2M0"]  = statusR2M0;
        ctx["RstatusR2M0"] = RstatusR2M0;
        ctx["statusR2M1"]  = statusR2M1;
        ctx["RstatusR2M1"] = RstatusR2M1;

        
        // Генерация страницы с использованием Mustache шаблона
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

std::map<std::string, uint8_t> parseKeyValueString(const std::string& input) {
    std::map<std::string, uint8_t> result;
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

RequestUATR map_to_RequestUATR(const std::map<std::string, uint8_t>& map) {
    // если нет ключей
    if (map.size() == 0) {
        CROW_LOG_WARNING << "map_to_RequestUATR: нет ключей";
        return RequestUATR();
    // если ключ 1
    } else if (map.size() == 1) {
        // если реле №0
        if (map.count("R0") == 1) {
            return RequestUATR(0, (map.at("R0")));
        // если реле №1
        } else if (map.count("R1") == 1) {
            // усли нужно включить или выключить реле №1
            if (map.at("R1") < 2) {
                return RequestUATR(1, map.at("R1"));
            // усли нужно включить реле №1 до времени х
            } else {
                uint hour;
                uint minute;
                time_server(hour, minute);
                uint temp = hour * 60 + minute + map.at("R1");
                return RequestUATR(1, 0, (uint8_t)(temp / 60), (uint8_t)(temp % 60));
            }
        // если реле №2
        } else if (map.count("R2") == 1) {
            return RequestUATR(2, map.at("R2"));
        // если реле №2 мод №0
        } else if (map.count("R2M0") == 1) {
            return RequestUATR(2, 0, map.at("R2M0"));
        // если реле №2 мод №1
        } else if (map.count("R2M1") == 1) {
            return RequestUATR(2, 1, map.at("R2M1"));
        // если не верные ключи
        } else {
            CROW_LOG_WARNING << "map_to_RequestUATR: нет искомых ключей";
            return RequestUATR();
        }
    // если ключей больше 2
    } else {
        // если есть ключ install
        if (map.count("install") == 1) {
            // если нужно изменить режим 0 реле №2
            if (map.at("install") == 0 && map.count("H_on") == 1 && map.count("M_on") == 1 &&
                    map.count("H_off") == 1 && map.count("M_off") == 1) {
                return RequestUATR(2, 0, map.at("M_on"), map.at("H_on"), map.at("M_off"), map.at("H_off"));
            // если нужно изменить режим 1 реле №2
            } else if (map.at("install") == 1 && map.count("H_on") == 1 && map.count("M_on") == 1 &&
                    map.count("H_off") == 1 && map.count("M_off") == 1) {
                return RequestUATR(2, 1, map.at("M_on"), map.at("H_on"), map.at("M_off"), map.at("H_off"));
            // если нужно установить время на плате
            } else if (map.at("install") == 2 && map.count("hour") == 1 && map.count("minute") == 1 &&
                map.count("day") == 1 && map.count("month") == 1 && map.count("year")) {
                return RequestUATR(map.at("minute"), map.at("hour"), map.at("day"), map.at("month"), map.at("year") - 2000);
            // если не верные ключи
            } else {
                CROW_LOG_WARNING << "map_to_RequestUATR: не верные данные";
                return RequestUATR();
            }
        // если нет ключя install
        } else {
            CROW_LOG_WARNING << "map_to_RequestUATR: нет ключя install";
            return RequestUATR();
        }
    }
}

// функция получения времени сервера
void time_server(uint & hour, uint & minute) {
    // Получаем текущее время с помощью chrono
    auto now = std::chrono::system_clock::now();
    // Преобразуем текущее время в time_t
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    // Преобразуем time_t в структуру tm для получения часов и минут
    std::tm* now_tm = std::localtime(&now_c);
    // Получаем часы и минуты
    hour = now_tm->tm_hour;
    minute = now_tm->tm_min;
}