#pragma once

#include <string>
#include <crow.h>

enum class RequestType {PING, PIN_ON, PIN_ON_MIN, PIN_OFF,
                        MOD_ON, MOD_OFF, MOD_T, TIME};

class RequestUATR {
    private:
        std::uint8_t flag     = 10;
        std::uint8_t relay    = 0;
        std::uint8_t mod      = 0;
        std::uint8_t min_on   = 0;
        std::uint8_t hour_on  = 0;
        std::uint8_t min_off  = 0;
        std::uint8_t hour_off = 0;
        std::uint8_t day      = 0;
        std::uint8_t month    = 0;
        std::uint8_t year     = 0;
    public:
        // запрос состояния всей платы
        RequestUATR();
        // изменение состояния реле по номеру
        RequestUATR(std::uint8_t relay, bool on);
        // включение реле до нужного времени
        RequestUATR(std::uint8_t relay, std::uint8_t mod, std::uint8_t hour, std::uint8_t min);
        // изменение состояния режима реле по номеру и моду
        RequestUATR(std::uint8_t relay, std::uint8_t mod, bool on);
        // изменение настроек режима по номеру и моду
        RequestUATR(std::uint8_t relay, std::uint8_t mod, std::uint8_t on_min, std::uint8_t on_hour,
                                                   std::uint8_t off_min, std::uint8_t off_hour);
        // установка времени на плате
        RequestUATR(std::uint8_t min, std::uint8_t hour,
                      std::uint8_t day, std::uint8_t month, std::uint8_t year);
        // сериализация класса в буфер для отправки по UART
        bool serialize(std::uint8_t * buffer, std::uint8_t size_buffer) const;
        // отображение параметров класса
        std::string show_request() const;
};