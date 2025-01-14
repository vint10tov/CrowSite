#include "url_relay.hpp"

URLRelay::URLRelay(Uart * uart) {
    this->uart = uart;
    RequestUATR r_uart;
    fill_strings_for_template(r_uart);
}

URLRelay::URLRelay(Uart * uart, std::string body) {
    this->uart = uart;
    // парсинг ключ-значение из тела запроса
    auto result = parseKeyValueString(body);
    // Вывод результата
    for (const auto& pair : result) {
        CROW_LOG_INFO << "Тело запроса: " << pair.first << " = " << (int)pair.second;
    }
    // объект запроса для платы
    RequestUATR r_uart = map_to_RequestUATR(result);
    fill_strings_for_template(r_uart);
}

void URLRelay::time_server(uint & hour, uint & minute) {
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

bool URLRelay::isInteger(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

std::map<std::string, std::uint8_t> URLRelay::parseKeyValueString(const std::string& input) {
    std::map<std::string, std::uint8_t> result;
    std::istringstream stream(input);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        std::string key;
        std::string value;

        std::size_t equalsPos = pair.find('=');
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

RequestUATR URLRelay::map_to_RequestUATR(const std::map<std::string, std::uint8_t>& map) {
    // Проверка наличия ключей
    if (map.empty()) {
        CROW_LOG_WARNING << "map_to_RequestUATR: нет ключей";
        return RequestUATR();
    }

    // Определяем ключи для реле
    auto getRelayValue = [&](const std::string& relayKey) {
        return map.count(relayKey) == 1 ? map.at(relayKey) : -1;
    };

    // Основная логика для 1 ключа
    if (map.size() == 1) {
        if (auto vR0 = getRelayValue("R0"); vR0 != -1) {
            return RequestUATR(0, vR0);
        } else if (auto vR1 = getRelayValue("R1"); vR1 != -1) {
            if (vR1 < 2) {
                return RequestUATR(1, vR1);
            } else {
                uint hour, minute;
                time_server(hour, minute);
                uint temp = hour * 60 + minute + vR1;
                return RequestUATR(1, 0, static_cast<std::uint8_t>(temp / 60), static_cast<std::uint8_t>(temp % 60));
            }
        } else if (auto vR2 = getRelayValue("R2"); vR2 != -1) {
            return RequestUATR(2, vR2);
        } else if (auto vR2M0 = getRelayValue("R2M0"); vR2M0 != -1) {
            return RequestUATR(2, 0, vR2M0);
        } else if (auto vR2M1 = getRelayValue("R2M1"); vR2M1 != -1) {
            return RequestUATR(2, 1, vR2M1);
        } else {
            CROW_LOG_WARNING << "map_to_RequestUATR: нет искомых ключей";
            return RequestUATR();
        }
    }

    // Обработка случая, когда ключей больше 2
    if (!map.count("install")) {
        CROW_LOG_WARNING << "map_to_RequestUATR: нет ключа install";
        return RequestUATR();
    }

    // Логика для установки значений в зависимости от install
    uint8_t installMode = map.at("install");
    switch (installMode) {
        case 0:
            if (map.count("H_on") && map.count("M_on") && map.count("H_off") && map.count("M_off")) {
                return RequestUATR(2, 0, map.at("M_on"), map.at("H_on"), map.at("M_off"), map.at("H_off"));
            }
            break;
        case 1:
            if (map.count("H_on") && map.count("M_on") && map.count("H_off") && map.count("M_off")) {
                return RequestUATR(2, 1, map.at("M_on"), map.at("H_on"), map.at("M_off"), map.at("H_off"));
            }
            break;
        case 2:
            if (map.count("hour") && map.count("minute") && map.count("day") && map.count("month") && map.count("year")) {
                return RequestUATR(map.at("minute"), map.at("hour"), map.at("day"), map.at("month"), map.at("year") - 2000);
            }
            break;
        default:
            break;
    }

    CROW_LOG_WARNING << "map_to_RequestUATR: не верные данные";
    return RequestUATR();
}

void URLRelay::fill_strings_for_template(RequestUATR &r_uart) {
    if (!r_uart.serialize(bufer_out, SIZE_BUF_out)) {
        sft.error = "ERROR: ошибка сериализации RequestUATR";
        return;
    }

    if (!uart->sending_string(bufer_in, bufer_out, SIZE_BUF_in, SIZE_BUF_out)) {
        sft.error = "ERROR: ошибка отправки или принятия данных";
        return;
    }

    if (!s_home.deserialize(bufer_in, SIZE_BUF_in)) {
        sft.error = "ERROR: ошибка десериализации SmartHome";
        return;
    }

    sft.error = "OK";
    sft.time = s_home.showTime();
    sft.timeR2M0ON = s_home.showTimeModOn(2, 0);
    sft.timeR2M0OFF = s_home.showTimeModOff(2, 0);
    sft.timeR2M1ON = s_home.showTimeModOn(2, 1);
    sft.timeR2M1OFF = s_home.showTimeModOff(2, 1);

    if (s_home.GET_status_relay(0)) {
        sft.statusR0 = "OFF";
        sft.RstatusR0 = "0";
    } else {
        sft.statusR0 = "ON";
        sft.RstatusR0 = "1";
    }
    if (s_home.GET_status_relay(1)) {
        sft.statusR1 = "OFF";
        sft.RstatusR1 = "0";
    } else {
        sft.statusR1 = "ON";
        sft.RstatusR1 = "1";
    }
    if (s_home.GET_status_relay(2)) {
        sft.statusR2 = "OFF";
        sft.RstatusR2 = "0";
    } else {
        sft.statusR2 = "ON";
        sft.RstatusR2 = "1";
    }
    if (s_home.GET_status_mod(2, 0)) {
        sft.statusR2M0 = "DEACT";
        sft.RstatusR2M0 = "0";
    } else {
        sft.statusR2M0 = "ACT";
        sft.RstatusR2M0 = "1";
    }
    if (s_home.GET_status_mod(2, 1)) {
        sft.statusR2M1 = "DEACT";
        sft.RstatusR2M1 = "0";
    } else {
        sft.statusR2M1 = "ACT";
        sft.RstatusR2M1 = "1";
    }
}