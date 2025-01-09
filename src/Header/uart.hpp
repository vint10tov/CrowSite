#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstdint>

class UartUno final {
    private:
        static UartUno* instance;          // Указатель на единственный экземпляр
        static std::mutex mutex_uno;       // Мьютекс для защиты многопоточности
        int fd = -1;
        const char port_name_0[13] = "/dev/ttyACM0";
        const char port_name_1[13] = "/dev/ttyACM1";

        // Закрытый конструктор для предотвращения создания экземпляров
        UartUno();
        // Закрытый деструктор
        ~UartUno();
        uint8_t calculate_checksum(const uint8_t *data, size_t size) const;
        bool verify_checksum(const uint8_t *data, size_t size) const;
    public:
        // Метод для получения единственного экземпляра класса
        static UartUno* getInstance();

        // Метод для отправки строки в порт и чтения строки из порта
        bool sending_string(uint8_t * buffer_in, uint8_t * buffer_out,
                             size_t size_buffer_in, size_t size_buffer_out);

        // Метод для проверки, открыт ли порт
        bool isOpen() const;
};