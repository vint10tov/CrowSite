# Указываем минимальную версию CMake
cmake_minimum_required(VERSION 3.10)

# Указываем имя проекта
project(ServerVintCrow)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

# Находим исходные файлы
file(GLOB CPP_SOURCES "src/Source/*.cpp")

# Указываем путь к заголовочным файлам
include_directories(src/Header/)
include_directories(/usr/include/postgresql)  # Добавляем путь к заголовочным файлам PostgreSQL
include_directories(${OPENSSL_INCLUDE_DIR})

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${CPP_SOURCES})

# Находим и подключаем библиотеки потоков
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} pqxx pq OpenSSL::SSL OpenSSL::Crypto)

# Указываем путь к библиотекам PostgreSQL
link_directories(/usr/lib/postgresql)  # Добавляем путь к библиотекам PostgreSQL