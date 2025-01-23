#pragma once

#include <crow.h>
#include <xlnt/xlnt.hpp>
#include <string>
#include <vector>

class Price {
    public:
        Price() {}
        Price(const std::string& filename);
        ~Price() {}

        struct Service
        {
            int id;                          // №
            std::string name;                // Наименование услуги
            std::string unit_of_measurement; // Ед. Из.
            int price;                       // Цена за ед.
        };
        // Количество услуг
        uint size() const { return services.size(); }
        // Добавление услуги
        void push_back(uint id, const std::string& name,
                       const std::string& unit_of_measurement, int price);
        // Экспорт данных
        void export_to_excel(const std::string& filename) const;
        // Импорт данных
        void import_from_excel(const std::string& filename);
        // Получение массива услуг
        const std::vector<Service> & get_services() const { return services; }
    private:
        std::vector<Service> services;
        // Проверка строки на целое число
        int convertToInt(const std::string& str) const;
};