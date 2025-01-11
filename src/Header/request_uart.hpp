#pragma once

#include <cstdint>
#include <string>

enum class RequestType {PING, PIN_ON, PIN_ON_MIN, PIN_OFF,
                        MOD_ON, MOD_OFF, MOD_T, TIME};

class RequestUATR {
    private:
        uint8_t flag     = 10;
        uint8_t relay    = 0;
        uint8_t mod      = 0;
        uint8_t min_on   = 0;
        uint8_t hour_on  = 0;
        uint8_t min_off  = 0;
        uint8_t hour_off = 0;
        uint8_t day      = 0;
        uint8_t month    = 0;
        uint8_t year     = 0;
    public:
        // запрос состояния всей платы
        RequestUATR();
        // изменение состояния реле по номеру
        RequestUATR(uint8_t relay, bool on);
        // включение реле до нужного времени
        RequestUATR(uint8_t relay, uint8_t mod, uint8_t hour, uint8_t min);
        // изменение состояния режима реле по номеру и моду
        RequestUATR(uint8_t relay, uint8_t mod, bool on);
        // изменение настроек режима по номеру и моду
        RequestUATR(uint8_t relay, uint8_t mod, uint8_t on_min, uint8_t on_hour,
                                                   uint8_t off_min, uint8_t off_hour);
        // установка времени на плате
        RequestUATR(uint8_t min, uint8_t hour,
                      uint8_t day, uint8_t month, uint8_t year);
        // сериализация класса в буфер для отправки по UART
        bool serialize(uint8_t * buffer, uint8_t size_buffer) const;
        // отображение параметров класса
        std::string show_request() const;
};