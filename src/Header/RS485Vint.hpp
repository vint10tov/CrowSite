#pragma once

#include <crow.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <mutex>

#include "config.hpp"

class RS485Vint final {
    private:
        static bool object_exists;     // Существование объекта этого класса в программе
        bool status_object = false;    // Статус объекта (fals - не активен)
        static std::mutex mutex;       // Мьютекс для защиты многопоточности
        int fd = -1;
        //const char port_name_0[13] = "/dev/ttyACM0";
        //const char port_name_1[13] = "/dev/ttyACM1";
        //const char port_name_2[13] = "/dev/ttyUSB0";
        //const char port_name_3[13] = "/dev/ttyUSB1";

        // Запрет на копирование экземпляров
        RS485Vint(const RS485Vint&) = delete;
        RS485Vint& operator=(const RS485Vint&) = delete;

        std::uint8_t calculate_checksum(const std::uint8_t *data, std::uint8_t size) const;
        bool verify_checksum(const std::uint8_t *data, std::uint8_t size) const;
    public:
        RS485Vint(Config & conf);
        ~RS485Vint();
        // Метод для отправки массива байт в порт и чтения массива из порта
        // последний байт - контросьная сумма
        bool sending_string(std::uint8_t * buffer_in, std::uint8_t * buffer_out,
                             std::uint8_t size_buffer_in, std::uint8_t size_buffer_out);
};