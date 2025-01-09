#include "uart.hpp"

std::mutex UartUno::mutex_uno;

// при создании указывается последовательный порт например "/dev/ttyUSB0"
UartUno::UartUno() {
    fd = open(port_name_0, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) {
        fd = open(port_name_1, O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) {
            //Logger::error_log("UartUno: ошибка открытия порта");
            return;
        }
    }

    struct termios options;
    tcgetattr(fd, &options);

    // Устанавливаем параметры порта
    cfsetispeed(&options, B9600); // Скорость приема
    cfsetospeed(&options, B9600); // Скорость передачи

    options.c_cflag |= (CLOCAL | CREAD); // Игнорируем режим управления модемом, включаем прием
    options.c_cflag &= ~PARENB;  // Без контроля четности
    options.c_cflag &= ~CSTOPB;  // Один стоп-бит
    options.c_cflag &= ~CSIZE;   // Очистить маску размера бита
    options.c_cflag |= CS8;      // Установить 8 бит данных
    options.c_cflag &= ~CRTSCTS; // Без аппаратного управления

    // Устанавливаем опции терминала
    options.c_lflag &= ~(ICANON | ECHO); // Режим неканонической линии, выключаем эхо
    options.c_oflag &= ~OPOST; // Выключаем постобработку вывода

    // Сохраняем настройки
    tcsetattr(fd, TCSANOW, &options);
    //Logger::info_log("UartUno: Последовательный порт открыт");
}

// Закрытый деструктор
UartUno::~UartUno() {
    if (isOpen()) {
        // Закрываем порт
        close(fd);
        delete instance;
        //Logger::info_log("UartUno: Последовательный порт закрыт");
    }
}

// Метод для получения единственного экземпляра класса
UartUno* UartUno::getInstance() {
    std::lock_guard<std::mutex> lock(mutex_uno); // Защита от многопоточного доступа
    if (instance == nullptr) {
        instance = new UartUno();
    }
    return instance;
}

// Метод для проверки, открыт ли порт
bool UartUno::isOpen() const {
    if (fd != -1)
        return true;
    else
        return false;
}

// Метод для отправки строки в порт и чтения строки из порта
bool UartUno::sending_string(uint8_t * buffer_in, uint8_t * buffer_out,
                             size_t size_buffer_in, size_t size_buffer_out) {

    std::lock_guard<std::mutex> lock(mutex_uno); // Защита от многопоточного доступа
    if (isOpen()) {
        // Вычисление и добавление контрольной суммы к buffer_in
        uint8_t checksum = calculate_checksum(buffer_out, size_buffer_out - 1);
        buffer_out[size_buffer_out - 1] = checksum; // Добавляем контрольную сумму в конец
        
        // Отправляем данные на Arduino
        ssize_t result = write(fd, buffer_out, size_buffer_out);
        if (result == -1) {
            // Обработка ошибки
            //Logger::error_log("UartUno: Ошибка записи");
            return false;
        }
        // Ждем немного перед чтением (можно настроить)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Чтение ответа от Arduino
        int bytesRead = read(fd, buffer_in, size_buffer_in);
        if (bytesRead < 0) {
            //Logger::error_log("UartUno: Ошибка чтения");
            return false;
        } else if (bytesRead == 0) {
            //Logger::error_log("UartUno: Нет данных");
            return false;
        }
        
        // Проверяем контрольную сумму полученных данных
        if (!verify_checksum(buffer_in, size_buffer_in)) {
            //Logger::error_log("UartUno: Ошибка контрольной суммы");
            return false;
        }

        return true;
    } else {
        return false;
    }
}

uint8_t UartUno::calculate_checksum(const uint8_t *data, size_t size) const {
    uint8_t checksum = 0;
    for (size_t i = 0; i < size; ++i) {
        checksum ^= data[i]; // Используем XOR для контроля
    }
    return checksum;
}

bool UartUno::verify_checksum(const uint8_t *data, size_t size) const {
    uint8_t received_checksum = data[size - 1]; // Предполагаем, что последний байт — это контрольная сумма
    return received_checksum == calculate_checksum(data, size - 1);
}