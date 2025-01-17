#pragma once

#include <crow.h>
#include <string>

const std::uint8_t BIT_7 = 128;    // 0b1000 0000
const std::uint8_t BIT_6 = 64;     // 0b0100 0000
const std::uint8_t BIT_5 = 32;     // 0b0010 0000
const std::uint8_t BIT_4 = 16;     // 0b0001 0000
const std::uint8_t BIT_3 = 8;      // 0b0000 1000
const std::uint8_t BIT_2 = 4;      // 0b0000 0100
const std::uint8_t BIT_1 = 2;      // 0b0000 0010

class SmartHome {
    private:
        // флаги
        // [7] изменения в классе
        // [6] статус relay 0
        // [5] статус relay 1
        // [4] статус relay 2
        // [3] статус relay 1 мод
        // [2] статус relay 2 мод 0
        // [1] статус relay 2 мод 1
        // [0]
        std::uint8_t flag = 0;

        std::uint8_t relay_0 = 0;
        std::uint8_t relay_1 = 0;
        std::uint8_t relay_2 = 0;

        // время на плате
        std::uint8_t second = 0; 
        std::uint8_t minute = 0;
        std::uint8_t hour = 0;
        std::uint8_t date = 0;
        std::uint8_t month = 0;
        std::uint8_t year = 0; // без 2000

        // relay 1
        std::uint16_t minute_off_relay_1 = 0;

        // relay 2
        std::uint16_t minute_on_relay_2_mod_0 = 0;
        std::uint16_t minute_off_relay_2_mod_0 = 0;
        std::uint16_t minute_on_relay_2_mod_1 = 0;
        std::uint16_t minute_off_relay_2_mod_1 = 0;
    public:
        SmartHome(std::uint8_t i = 0);
        // Запрос статуса реле
        bool GET_status_relay(std::uint8_t relay) const;
        // Запрос статуса мод
        bool GET_status_mod(std::uint8_t relay, uint8_t mod) const;
        // Запрос пина
        std::uint8_t GET_pin(std::uint8_t relay) const;
        // Запрос времени <on>
        std::uint16_t GET_minut_on(std::uint8_t relay, std::uint8_t mod) const;
        // Запрос времени <off>
        std::uint16_t GET_minut_off(std::uint8_t relay, std::uint8_t mod) const;
        // Десериализация из бинарного формата
        bool deserialize(const std::uint8_t * buffer, std::uint8_t size_buffer);
        std::string showTime() const;
        std::string showTimeModOn(std::uint8_t relay, std::uint8_t mod);
        std::string showTimeModOff(std::uint8_t relay, std::uint8_t mod);
};