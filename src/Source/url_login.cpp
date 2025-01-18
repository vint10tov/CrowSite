#include "url_login.hpp"

URLLogin::URLLogin(std::string body, ConnectDB & db) {
    // парсинг ключ-значение из тела запроса
    auto result = parseKeyValueString(body);
    
    if (containsSqlInjection(result.at("username"))) {
        CROW_LOG_INFO << "URLLogin: Обнаружена SQL-инъекция";
        return;
    }

    if (!db.get_user(result.at("username"), user)) {
        CROW_LOG_INFO << "URLLogin: Пользователя " << result.at("username") << " не существует";
        return;
    }
    
    if (hash_password(result.at("psw"), user.salt) != user.psw) {
        CROW_LOG_INFO << "URLLogin: Не верный пароль";
        CROW_LOG_INFO << hash_password(result.at("psw"), user.salt);
        CROW_LOG_INFO << user.psw;
        return;
    }
    check_login = true;
}

std::map<std::string, std::string> URLLogin::parseKeyValueString(const std::string& input) {
    std::map<std::string, std::string> result;
    std::istringstream stream(input);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        std::string key;
        std::string value;

        size_t equalsPos = pair.find('=');
        if (equalsPos != std::string::npos) {
            key = pair.substr(0, equalsPos);
            value = pair.substr(equalsPos + 1);
        } else {
            key = pair;
            value = "";
        }
        result[key] = value;
    }

    return result;
}

std::string URLLogin::generate_salt(std::size_t length) {
    const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, sizeof(chars) - 2);

    std::string salt;
    for (std::size_t i = 0; i < length; ++i) {
        salt += chars[distribution(generator)];
    }
    return salt;
}

std::string URLLogin::hash_password(const std::string &password, const std::string &salt) {
    std::string salted_password = password + salt;

    unsigned char hashed[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(salted_password.c_str()), salted_password.size(), hashed);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hashed[i]);
    }
    return oss.str();
}

bool URLLogin::containsSqlInjection(const std::string& input) {
    const std::string sqlSpecialChars = "'\";--";
    for (char c : sqlSpecialChars) {
        if (input.find(c) != std::string::npos) {
            return true; // Найден специальный символ
        }
    }
    return false; // SQL-инъекций нет
}