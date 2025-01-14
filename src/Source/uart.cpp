#include "uart.hpp"

// при создании указывается последовательный порт например "/dev/ttyUSB0"
Uart::Uart() {
    fd = open(port_name_2, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) {
        fd = open(port_name_3, O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) {
            CROW_LOG_ERROR << "Uart: ошибка открытия порта";
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
    CROW_LOG_INFO << "UartUno: Последовательный порт открыт";
}

// Закрытый деструктор
Uart::~Uart() {
    if (isOpen()) {
        // Закрываем порт
        close(fd);
        CROW_LOG_INFO << "UartUno: Последовательный порт закрыт";
    }
    delete instance;
}

// Метод для получения единственного экземпляра класса
Uart* Uart::getInstance() {
    std::lock_guard<std::mutex> lock(mutex); // Защита от многопоточного доступа
    if (instance == nullptr) {
        instance = new Uart();
    }
    return instance;
}

// Метод для проверки, открыт ли порт
bool Uart::isOpen() const {
    if (fd != -1)
        return true;
    else
        return false;
}

// Метод для отправки строки в порт и чтения строки из порта
bool Uart::sending_string(std::uint8_t * buffer_in, std::uint8_t * buffer_out,
                             std::uint8_t size_buffer_in, std::uint8_t size_buffer_out) {

    std::lock_guard<std::mutex> lock(mutex); // Защита от многопоточного доступа
    if (isOpen()) {
        clear_buffer();
        // Вычисление и добавление контрольной суммы к buffer_in
        uint8_t checksum = calculate_checksum(buffer_out, size_buffer_out - static_cast<std::uint8_t>(1));
        buffer_out[size_buffer_out - static_cast<std::uint8_t>(1)] = checksum; // Добавляем контрольную сумму в конец
        
        // Отправляем данные на Arduino
        ssize_t result = write(fd, buffer_out, size_buffer_out);
        if (result == -1) {
            // Обработка ошибки
            CROW_LOG_ERROR << "Uart: Ошибка записи";
            return false;
        }
        // Ждем немного перед чтением (можно настроить)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Чтение ответа от Arduino
        int bytesRead = read(fd, buffer_in, size_buffer_in);
        //CROW_LOG_INFO << "UartUno: read = " << bytesRead;
        if (bytesRead < 0) {
            CROW_LOG_ERROR << "Uart: Ошибка чтения";
            return false;
        }
        if (bytesRead == 0) {
            CROW_LOG_ERROR << "Uart: Нет данных";
            return false;
        }
        if (bytesRead != static_cast<int>(size_buffer_in)) {
            CROW_LOG_ERROR << "Uart: Не верное количество байт: " << bytesRead;
            return false;
        }
        if (!verify_checksum(buffer_in, size_buffer_in)) {
            CROW_LOG_ERROR << "Uart: Не верная контрольная сумма";
            return false;
        }

        return true;
    } else {
        return false;
    }
}

std::uint8_t Uart::calculate_checksum(const std::uint8_t *data, std::uint8_t size) const {
    std::uint8_t checksum = 0;
    for (std::uint8_t i = 0; i < size; ++i) {
        checksum ^= data[i]; // Используем XOR для контроля
    }
    return checksum;
}

bool Uart::verify_checksum(const std::uint8_t *data, std::uint8_t size) const {
    std::uint8_t received_checksum = data[size - static_cast<std::uint8_t>(1)]; // Предполагаем, что последний байт — это контрольная сумма
    return received_checksum == calculate_checksum(data, size - static_cast<std::uint8_t>(1));
}

// Метод для очистки буфера порта
void Uart::clear_buffer() {
    if (isOpen()) {
        // Чистим вводной и выводной буферы
        tcflush(fd, TCIOFLUSH);
    }
}