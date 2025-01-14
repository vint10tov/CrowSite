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

class Uart final {
    private:
        static Uart* instance;         // Указатель на единственный экземпляр
        static std::mutex mutex;       // Мьютекс для защиты многопоточности
        int fd = -1;
        const char port_name_0[13] = "/dev/ttyACM0";
        const char port_name_1[13] = "/dev/ttyACM1";
        const char port_name_2[13] = "/dev/ttyUSB0";
        const char port_name_3[13] = "/dev/ttyUSB1";

        // Закрытый конструктор для предотвращения создания экземпляров
        Uart();
        // Закрытый деструктор
        ~Uart();
        std::uint8_t calculate_checksum(const std::uint8_t *data, std::uint8_t size) const;
        bool verify_checksum(const std::uint8_t *data, std::uint8_t size) const;
        void clear_buffer();
    public:
        // Метод для получения единственного экземпляра класса
        static Uart* getInstance();

        // Метод для отправки строки в порт и чтения строки из порта
        bool sending_string(std::uint8_t * buffer_in, std::uint8_t * buffer_out,
                             std::uint8_t size_buffer_in, std::uint8_t size_buffer_out);

        // Метод для проверки, открыт ли порт
        bool isOpen() const;
};