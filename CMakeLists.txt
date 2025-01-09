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

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${CPP_SOURCES})

# Указываем путь к заголовочным файлам
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/Header/
    /usr/include/
)

# Находим и подключаем библиотеки потоков
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
