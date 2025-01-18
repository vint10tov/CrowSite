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
        static std::mutex mutex;       // Мьютекс для защиты многопоточности
        int fd = -1;
        Config * conf;

        // создание дескриптора порта
        bool open_port();
        // настройка порта
        void set_port();
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