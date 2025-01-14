#pragma once

#include <crow.h>
#include <string>
#include <ctime>
#include <chrono>
#include <map>
#include <sstream>
#include <ctime>
#include <chrono>

#include "uart.hpp"
#include "request_uart.hpp"
#include "smart_home.hpp"
#include "uart.hpp"

class URLRelay {

    static const std::uint8_t SIZE_BUF_in  = 21;
    static const std::uint8_t SIZE_BUF_out = 11;

    public:
        struct StringsForTemplate {
            //строки для шаблонизатора
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
        };

        URLRelay(Uart * uart);
        URLRelay(Uart * uart, std::string body);
        const StringsForTemplate & GET_StringsForTemplate() const {return sft;}

    private:
        StringsForTemplate sft;
        std::uint8_t bufer_in[SIZE_BUF_in]   = {0};
        std::uint8_t bufer_out[SIZE_BUF_out] = {0};
        SmartHome s_home;
        Uart * uart = nullptr;

        // функция получения времени linux
        void time_server(uint & hour, uint & minute);
        // тело запроса в ключ-значение
        std::map<std::string, std::uint8_t> parseKeyValueString(const std::string& input);
        // проверка строки на возможность преобразования в число
        bool isInteger(const std::string& str);
        // ключ-значение в RequestUATR
        RequestUATR map_to_RequestUATR(const std::map<std::string, std::uint8_t>& map);
        // функция заполнения strings_for_template
        void fill_strings_for_template(RequestUATR & r_uart);
};