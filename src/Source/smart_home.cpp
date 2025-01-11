#include <crow.h>

#include "smart_home.hpp"

SmartHome::SmartHome(uint8_t i) {
    flag = i;
    relay_0 = i;
    relay_1 = i;
    relay_2 = i;
    second = i;
    minute = i;
    hour = i;
    date = i;
    month = i;
    year = i;
    minute_off_relay_1 = i;
    minute_on_relay_2_mod_0 = i;
    minute_off_relay_2_mod_0 = i;
    minute_on_relay_2_mod_1 = i;
    minute_off_relay_2_mod_1 = i;
}

// Запрос статуса реле
bool SmartHome::GET_status_relay(uint8_t relay) const {
    if (relay == 0)
        return ((flag & BIT_6) >> 6);
    else if (relay == 1)
        return ((flag & BIT_5) >> 5);
    else if (relay == 2)
        return ((flag & BIT_4) >> 4);
    else
        return false;
}

// Запрос статуса мод
bool SmartHome::GET_status_mod(uint8_t relay, uint8_t mod) const {
    if (relay == 1)
        return ((flag & BIT_3) >> 3);
    else if (relay == 2 && mod == 0)
        return ((flag & BIT_2) >> 2);
    else if (relay == 2 && mod == 1)
        return ((flag & BIT_1) >> 1);
    else
        return false;
}

// Запрос пина
uint8_t SmartHome::GET_pin(uint8_t relay) const {
    if (relay == 0)
        return relay_0;
    else if (relay == 1)
        return relay_1;
    else if (relay == 2)
        return relay_2;
    else
        return 0;
}

// Запрос времени <on>
uint16_t SmartHome::GET_minut_on(uint8_t relay, uint8_t mod) const {
    if (relay == 2 && mod == 0) {
        return minute_on_relay_2_mod_0;
    }
    else if (relay == 2 && mod == 1) {
        return minute_on_relay_2_mod_1;
    } else {
        return 0;
    }
}

// Запрос времени <off>
uint16_t SmartHome::GET_minut_off(uint8_t relay, uint8_t mod) const {
    if (relay == 1) {
        return minute_off_relay_1;
    }
    else if (relay == 2 && mod == 0) {
        return minute_off_relay_2_mod_0;
    }
    else if (relay == 2 && mod == 1) {
        return minute_off_relay_2_mod_1;
    } else {
        return 0;
    }
}

bool SmartHome::deserialize(const uint8_t *buffer, uint8_t size_buffer) {
    if (size_buffer < 20) {
        CROW_LOG_ERROR << "SmartHome: Невозможно десериализовать данные";
        return false;
    }
    flag    = buffer[0];
    relay_0 = buffer[1];
    relay_1 = buffer[2];
    relay_2 = buffer[3];
    second  = buffer[4];
    minute  = buffer[5];
    hour    = buffer[6];
    date    = buffer[7];
    month   = buffer[8];
    year    = buffer[9];
    minute_off_relay_1 = (buffer[11] << 8) | buffer[10];
    minute_on_relay_2_mod_0  = (buffer[13] << 8) | buffer[12];
    minute_off_relay_2_mod_0 = (buffer[15] << 8) | buffer[14];
    minute_on_relay_2_mod_1  = (buffer[17] << 8) | buffer[16];
    minute_off_relay_2_mod_1 = (buffer[19] << 8) | buffer[18];
    return true;
}

std::string SmartHome::showTime() const {
    char dates[32];
    int index = 0;
    // часы
    if (hour >= 0 && hour < 24) {
        dates[index++] = hour / 10 + 48; // [0]
        dates[index++] = hour % 10 + 48; // [1]
    } else {
        dates[index++] = '?';            // [0]
        dates[index++] = '?';            // [1]
    }
    dates[index++] = ':';
    // минуты
    if (minute >= 0 && minute < 60) {
        dates[index++] = minute / 10 + 48; // [2]
        dates[index++] = minute % 10 + 48; // [3]
    } else {
        dates[index++] = '?';              // [2]
        dates[index++] = '?';              // [3]
    }
    dates[index++] = ':';
    // секунды
    if (second >= 0 && second < 60) {
        dates[index++] = second / 10 + 48; // [4]
        dates[index++] = second % 10 + 48; // [5]
    } else {
        dates[index++] = '?';              // [4]
        dates[index++] = '?';              // [5]
    }
    dates[index++] = ' ';
    // день
    if (date >= 0 && date < 10) {
        dates[index++] = date + 48;      // [6]
    } else if (date > 9 && date < 32) {
        dates[index++] = date / 10 + 48; // [6]
        dates[index++] = date % 10 + 48; // [7]
    } else {
        dates[index++] = '?';            // [6]
    }
    dates[index++] = ' ';
    // месяц
    if (month > 0 && month < 13) {
        switch (month) {
        case 1:
            dates[index++] = 'J';
            dates[index++] = 'a';
            dates[index++] = 'n';
            dates[index++] = 'u';
            dates[index++] = 'a';
            dates[index++] = 'r';
            dates[index++] = 'y';
            break;
        case 2:
            dates[index++] = 'F';
            dates[index++] = 'e';
            dates[index++] = 'b';
            dates[index++] = 'r';
            dates[index++] = 'u';
            dates[index++] = 'a';
            dates[index++] = 'r';
            dates[index++] = 'y';
            break;
        case 3:
            dates[index++] = 'M';
            dates[index++] = 'a';
            dates[index++] = 'r';
            dates[index++] = 'c';
            dates[index++] = 'h';
            break;
        case 4:
            dates[index++] = 'A';
            dates[index++] = 'p';
            dates[index++] = 'r';
            dates[index++] = 'i';
            dates[index++] = 'l';
            break;
        case 5:
            dates[index++] = 'M';
            dates[index++] = 'a';
            dates[index++] = 'y';
            break;
        case 6:
            dates[index++] = 'J';
            dates[index++] = 'u';
            dates[index++] = 'n';
            dates[index++] = 'e';
            break;
        case 7:
            dates[index++] = 'J';
            dates[index++] = 'u';
            dates[index++] = 'l';
            dates[index++] = 'e';
            break;
        case 8:
            dates[index++] = 'A';
            dates[index++] = 'u';
            dates[index++] = 'g';
            dates[index++] = 'u';
            dates[index++] = 's';
            dates[index++] = 't';
            break;
        case 9:
            dates[index++] = 'S';
            dates[index++] = 'e';
            dates[index++] = 'p';
            dates[index++] = 't';
            dates[index++] = 'e';
            dates[index++] = 'm';
            dates[index++] = 'b';
            dates[index++] = 'e';
            dates[index++] = 'r';
            break;
        case 10:
            dates[index++] = 'O';
            dates[index++] = 'c';
            dates[index++] = 't';
            dates[index++] = 'o';
            dates[index++] = 'b';
            dates[index++] = 'e';
            dates[index++] = 'r';
            break;
        case 11:
            dates[index++] = 'N';
            dates[index++] = 'o';
            dates[index++] = 'v';
            dates[index++] = 'e';
            dates[index++] = 'm';
            dates[index++] = 'b';
            dates[index++] = 'e';
            dates[index++] = 'r';
            break;
        case 12:
            dates[index++] = 'D';
            dates[index++] = 'e';
            dates[index++] = 'c';
            dates[index++] = 'e';
            dates[index++] = 'm';
            dates[index++] = 'b';
            dates[index++] = 'e';
            dates[index++] = 'r';
            break;
        
        default:
            break;
        }
    } else {
        dates[index++] = '?';
    }
    dates[index++] = ' ';
    // год
    dates[index++] = '2';
    dates[index++] = '0';
    if (year >= 0 && year < 100) {
        dates[index++] = year / 10 + 48;
        dates[index++] = year % 10 + 48;
    } else {
        dates[index++] = '?';
        dates[index++] = '?';
    }
    dates[index] = '\0';
    return dates;
}

std::string SmartHome::showTimeModOn(uint8_t relay, uint8_t mod) {
    char times[6];
    int index = 0;

    if (mod == 0) {
        // Вычисляем часы и минуты
        unsigned int hours = minute_on_relay_2_mod_0 / 60;
        unsigned int minutes = minute_on_relay_2_mod_0 % 60;
        if (hours >= 0 && hours < 24) {
            times[index++] = hours / 10 + 48;
            times[index++] = hours % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index++] = ':';
        if (minutes >= 0 && minutes < 60) {
            times[index++] = minutes / 10 + 48;
            times[index++] = minutes % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index] = '\0';
    } else if (mod == 1) {
        int hours = minute_on_relay_2_mod_1 / 60;
        int minutes = minute_on_relay_2_mod_1 % 60;
        if (hours >= 0 && hours < 24) {
            times[index++] = hours / 10 + 48;
            times[index++] = hours % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index++] = ':';
        if (minutes >= 0 && minutes < 60) {
            times[index++] = minutes / 10 + 48;
            times[index++] = minutes % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index] = '\0';
    } else {
        return "??:??";
    }
    return times;
}

std::string SmartHome::showTimeModOff(uint8_t relay, uint8_t mod) {
    char times[6];
    int index = 0;

    if (relay == 1) {
        // Вычисляем часы и минуты
        int hours = minute_off_relay_1 / 60;
        int minutes = minute_off_relay_1 % 60;
        if (hours >= 0 && hours < 24) {
            times[index++] = hours / 10 + 48;
            times[index++] = hours % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index++] = ':';
        if (minutes >= 0 && minutes < 60) {
            times[index++] = minutes / 10 + 48;
            times[index++] = minutes % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index] = '\0';
    } else if (relay == 2 && mod == 0) {
        // Вычисляем часы и минуты
        int hours = minute_off_relay_2_mod_0 / 60;
        int minutes = minute_off_relay_2_mod_0 % 60;
        if (hours >= 0 && hours < 24) {
            times[index++] = hours / 10 + 48;
            times[index++] = hours % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index++] = ':';
        if (minutes >= 0 && minutes < 60) {
            times[index++] = minutes / 10 + 48;
            times[index++] = minutes % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index] = '\0';
    } else if (relay == 2 && mod == 1) {
        int hours = minute_off_relay_2_mod_1 / 60;
        int minutes = minute_off_relay_2_mod_1 % 60;
        if (hours >= 0 && hours < 24) {
            times[index++] = hours / 10 + 48;
            times[index++] = hours % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index++] = ':';
        if (minutes >= 0 && minutes < 60) {
            times[index++] = minutes / 10 + 48;
            times[index++] = minutes % 10 + 48;
        } else {
            times[index++] = '?';
            times[index++] = '?';
        }
        times[index] = '\0';
    } else {
        return "??:??";
    }
    return times;
}