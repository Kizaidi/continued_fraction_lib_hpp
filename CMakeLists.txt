# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта и версия
project(continued_fraction_lib
        VERSION 1.0.0
        DESCRIPTION "Библиотека для работы с цепными дробями"
        LANGUAGES CXX
)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Список исходных файлов
set(SOURCES
        main.cpp
        continued_fraction.cpp
)

# Список заголовочных файлов (для IDE)
set(HEADERS
        continued_fraction.h
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Настройки компиляции для разных компиляторов
if(MSVC)
    # Microsoft Visual C++
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4     # Высокий уровень предупреждений
            /WX     # Обрабатывать предупреждения как ошибки
            /permissive- # Строгое соответствие стандарту
    )
else()
    # GCC/Clang
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall     # Все стандартные предупреждения
            -Wextra   # Дополнительные предупреждения
            -Wpedantic # Строгое соответствие стандарту
            -Werror   # Предупреждения как ошибки
    )
endif()

# Настройки оптимизации
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Режим отладки
    target_compile_options(${PROJECT_NAME} PRIVATE
            -g      # Отладочная информация
            -O0     # Без оптимизаций
            -DDEBUG # Определение для отладки
    )
    message(STATUS "Конфигурация сборки: Debug")
else()
    # Режим релиза
    target_compile_options(${PROJECT_NAME} PRIVATE
            -O2      # Оптимизация по скорости
            -DNDEBUG # Отключение утверждений
    )
    message(STATUS "Конфигурация сборки: Release")
endif()

# Добавление директории с заголовками
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Информация о сборке
message(STATUS "Проект: ${PROJECT_NAME}")
message(STATUS "Версия: ${PROJECT_VERSION}")
message(STATUS "Компилятор: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Стандарт C++: ${CMAKE_CXX_STANDARD}")

# Дополнительные цели (опционально)
add_custom_target(run
        COMMAND ${PROJECT_NAME}
        DEPENDS ${PROJECT_NAME}
        COMMENT "Запуск программы"
)

add_custom_target(clean-all
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
        COMMENT "Полная очистка сборки"
)
