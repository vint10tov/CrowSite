#include "RS485Vint.hpp"

bool RS485Vint::object_exists = false;
std::mutex RS485Vint::mutex;

// при создании указывается последовательный порт например "/dev/ttyUSB0"
RS485Vint::RS485Vint(Config & conf) {
    if (object_exists) {
        CROW_LOG_WARNING << "RS485Vint: объект уже существует";
        return;
    }
    status_object  = true;
    fd = open(conf.get_port_1().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) {
        fd = open(conf.get_port_2().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) {
            CROW_LOG_ERROR << "RS485Vint: ошибка открытия порта";
            return;
        }
    }

    struct termios options;
    tcgetattr(fd, &options);

    // Устанавливаем параметры порта
    cfsetispeed(&options, B9600); // Скорость приема
    cfsetospeed(&options, B9600); // Скорость передачи

    // Настройка параметров порта
    options.c_cflag = (options.c_cflag & ~CSIZE) | CS8; // 8 бит данных
    options.c_cflag |= (CLOCAL | CREAD); // Разрешить прием
    options.c_cflag &= ~PARENB; // Без четности
    options.c_cflag &= ~CSTOPB; // 1 стоп-бит
    options.c_cflag &= ~CRTSCTS; // Без аппаратного управления

    //options.c_cflag |= (CLOCAL | CREAD); // Игнорируем режим управления модемом, включаем прием
    //options.c_cflag &= ~PARENB;  // Без контроля четности
    //options.c_cflag &= ~CSTOPB;  // Один стоп-бит
    //options.c_cflag &= ~CSIZE;   // Очистить маску размера бита
    //options.c_cflag |= CS8;      // Установить 8 бит данных
    //options.c_cflag &= ~CRTSCTS; // Без аппаратного управления

    // Установка режима выхода
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Режим неканонического ввода
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Без программного управления потоком
    options.c_oflag &= ~OPOST; // Без обработки выходных данных

    // Устанавливаем опции терминала
    //options.c_lflag &= ~(ICANON | ECHO); // Режим неканонической линии, выключаем эхо
    //options.c_oflag &= ~OPOST; // Выключаем постобработку вывода

    // Сохраняем настройки
    tcsetattr(fd, TCSANOW, &options);
    CROW_LOG_INFO << "RS485Vint: Последовательный порт открыт";
}

// Закрытый деструктор
RS485Vint::~RS485Vint() {
    object_exists = false;
    // Закрываем порт
    if (fd > 0) {
        close(fd);
    }
}

// Метод для отправки строки в порт и чтения строки из порта
bool RS485Vint::sending_string(std::uint8_t * buffer_in, std::uint8_t * buffer_out,
                             std::uint8_t size_buffer_in, std::uint8_t size_buffer_out) {

    std::lock_guard<std::mutex> lock(mutex); // Защита от многопоточного доступа
    if (fd > 0) {
        tcflush(fd, TCIOFLUSH);
        // Вычисление и добавление контрольной суммы к buffer_in
        uint8_t checksum = calculate_checksum(buffer_out, size_buffer_out - static_cast<std::uint8_t>(1));
        buffer_out[size_buffer_out - static_cast<std::uint8_t>(1)] = checksum; // Добавляем контрольную сумму в конец
        
        // Отправляем данные на Arduino
        ssize_t result = write(fd, buffer_out, size_buffer_out);
        if (result == -1) {
            // Обработка ошибки
            CROW_LOG_ERROR << "RS485Vint: Ошибка записи";
            return false;
        }
        // Ждем немного перед чтением (можно настроить)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Чтение ответа от Arduino
        int bytesRead = read(fd, buffer_in, size_buffer_in);
        //CROW_LOG_INFO << "UartUno: read = " << bytesRead;
        if (bytesRead < 0) {
            CROW_LOG_ERROR << "RS485Vint: Ошибка чтения";
            return false;
        }
        if (bytesRead == 0) {
            CROW_LOG_ERROR << "RS485Vint: Нет данных";
            return false;
        }
        if (bytesRead != static_cast<int>(size_buffer_in)) {
            CROW_LOG_ERROR << "RS485Vint: Не верное количество байт: " << bytesRead;
            return false;
        }
        if (!verify_checksum(buffer_in, size_buffer_in)) {
            CROW_LOG_ERROR << "RS485Vint: Не верная контрольная сумма";
            return false;
        }

        return true;
    } else {
        CROW_LOG_ERROR << "RS485Vint: Порт закрыт";
        return false;
    }
}

std::uint8_t RS485Vint::calculate_checksum(const std::uint8_t *data, std::uint8_t size) const {
    std::uint8_t checksum = 0;
    for (std::uint8_t i = 0; i < size; ++i) {
        checksum ^= data[i]; // Используем XOR для контроля
    }
    return checksum;
}

bool RS485Vint::verify_checksum(const std::uint8_t *data, std::uint8_t size) const {
     // Предполагаем, что последний байт — это контрольная сумма
    std::uint8_t received_checksum = data[size - static_cast<std::uint8_t>(1)];
    return received_checksum == calculate_checksum(data, size - static_cast<std::uint8_t>(1));
}