#include "price.hpp"

Price::Price(const std::string& filename) {
    import_from_excel(filename);
}

void Price::push_back(uint id, const std::string& name,
                      const std::string& unit_of_measurement, int price) {
    Service temp;
    temp.id = id;
    temp.name = name;
    temp.unit_of_measurement = unit_of_measurement;
    temp.price = price;
    services.push_back(temp);
}

void Price::export_to_excel(const std::string& filename) const {
    if (services.empty()) {
        CROW_LOG_WARNING << "Price: Нет данных для экспорта";
        return;
    }
    try {
        xlnt::workbook wb;
        xlnt::worksheet ws = wb.active_sheet();

        // Заголовки
        ws.cell("A1").value("ID");
        ws.cell("B1").value("Наименование услуги");
        ws.cell("C1").value("Ед. Из.");
        ws.cell("D1").value("Цена за ед.");

        // Заполнение данных
        for (size_t i = 0; i < services.size(); ++i) {
            ws.cell("A" + std::to_string(i + 2)).value(services[i].id);
            ws.cell("B" + std::to_string(i + 2)).value(services[i].name);
            ws.cell("C" + std::to_string(i + 2)).value(services[i].unit_of_measurement);
            ws.cell("D" + std::to_string(i + 2)).value(services[i].price);
        }

        wb.save(filename);
    } catch (const std::exception &e) {
        CROW_LOG_ERROR << e.what();
        return;
    }
}

void Price::import_from_excel(const std::string& filename) {
    try {
        xlnt::workbook wb;
        wb.load(filename);
        xlnt::worksheet ws = wb.active_sheet();

        services.clear();

        for (auto row : ws.rows()) {
            // Пропустим заголовки
            if (row[0].row() == 1) continue;

            Service temp;
            temp.id                  = convertToInt(row[0].to_string());
            temp.name                = row[1].to_string();
            temp.unit_of_measurement = row[2].to_string();
            temp.price               = convertToInt(row[3].to_string());

            services.push_back(std::move(temp));
        }
    } catch (const std::exception &e) {
        CROW_LOG_ERROR << e.what();
        return;
    }
}

int Price::convertToInt(const std::string& str) const {
    // Проверяем на пустую строку
    if (str.empty()) {
        return 0; // Возвращаем 0 для пустой строки
    }

    // Убираем ведущие и замыкающие пробелы
    size_t idx;
    try {
        int result = std::stoi(str, &idx); // Пытаемся преобразовать строку в int
        // Проверяем, все ли символы были обработаны (чтобы избежать "12a" или "3.14")
        if (idx == str.length()) {
            return result;
        }
    } catch (const std::invalid_argument&) {
        return 0; // Строка не является корректным целым числом
    } catch (const std::out_of_range&) {
        return 0; // Число вне диапазона int
    }

    return 0; // Возвращаем 0, если преобразование не удалось
}