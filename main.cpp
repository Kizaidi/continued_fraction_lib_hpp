
/**
 * @file main.cpp
 * @brief Демонстрация использования библиотеки цепных дробей
 * @author КИЗАИДИ АНТОНИУ
 * @date 05/12/2025
 *
 * Примеры использования всех основных функций библиотеки:
 * - Создание объектов
 * - Арифметические операции
 * - Специальные функции
 * - Ввод/вывод
 */

#include "continued_fraction.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <windows.h>

using namespace Math;

/**
 * @brief Вывод заголовка раздела
 * @param title Название раздела
 */
void print_header(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

/**
 * @brief Пример 1: Создание цепных дробей различными способами
 */
void example_creation() {
    print_header("Пример 1: Создание цепных дробей");

    // 1. Из целого числа
    ContinuedFraction cf1(42);
    std::cout << "1. Из целого числа 42: " << cf1 << std::endl;

    // 2. Из вектора коэффициентов (приближение π)
    std::vector<long long> coeffs = {3, 7, 15, 1, 292};
    ContinuedFraction cf2(coeffs);
    std::cout << "2. Из вектора [3; 7, 15, 1, 292]: " << cf2 << std::endl;
    std::cout << "   Числовое значение: " << cf2.to_double() << std::endl;

    // 3. Из строки
    ContinuedFraction cf3("[1; 2, 3, 4]");
    std::cout << "3. Из строки \"[1; 2, 3, 4]\": " << cf3 << std::endl;

    // 4. Из рационального числа (приближение π)
    ContinuedFraction cf4 = ContinuedFraction::from_rational(355, 113);
    std::cout << "4. Из дроби 355/113: " << cf4 << std::endl;
    std::cout << "   Значение: " << cf4.to_double() << " (π ≈ " << 355.0/113.0 << ")" << std::endl;

    // 5. Из десятичного числа
    ContinuedFraction cf5 = ContinuedFraction::from_double(2.718281828459045, 10);
    std::cout << "5. Из числа e (10 коэффициентов): " << cf5 << std::endl;
    std::cout << "   Значение: " << cf5.to_double() << std::endl;
}

/**
 * @brief Пример 2: Арифметические операции
 */
void example_operations() {
    print_header("Пример 2: Арифметические операции");

    ContinuedFraction a("[1; 2, 3]");
    ContinuedFraction b("[2; 1, 4]");

    std::cout << "a = " << a << " ≈ " << a.to_double() << std::endl;
    std::cout << "b = " << b << " ≈ " << b.to_double() << std::endl;

    // Сложение
    ContinuedFraction sum = a + b;
    std::cout << "\na + b = " << sum << " ≈ " << sum.to_double() << std::endl;

    // Вычитание
    ContinuedFraction diff = a - b;
    std::cout << "a - b = " << diff << " ≈ " << diff.to_double() << std::endl;

    // Умножение
    ContinuedFraction prod = a * b;
    std::cout << "a * b = " << prod << " ≈ " << prod.to_double() << std::endl;

    // Деление
    try {
        ContinuedFraction quot = a / b;
        std::cout << "a / b = " << quot << " ≈ " << quot.to_double() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Ошибка при делении: " << e.what() << std::endl;
    }
}

/**
 * @brief Пример 3: Подходящие дроби
 */
void example_convergents() {
    print_header("Пример 3: Подходящие дроби для π");

    ContinuedFraction cf = ContinuedFraction::from_rational(355, 113);
    std::cout << "Цепная дробь для 355/113: " << cf << std::endl;

    std::cout << "\nПодходящие дроби:\n";
    std::cout << std::setw(5) << "n"
              << std::setw(15) << "Числитель"
              << std::setw(15) << "Знаменатель"
              << std::setw(20) << "Значение" << std::endl;

    for (size_t i = 0; i < cf.size(); ++i) {
        auto [num, den] = cf.convergent(i);
        double value = static_cast<double>(num) / den;

        std::cout << std::setw(5) << i
                  << std::setw(15) << num
                  << std::setw(15) << den
                  << std::setw(20) << std::setprecision(10) << value << std::endl;
    }
}

/**
 * @brief Пример 4: Специальные числа
 */
void example_special_numbers() {
    print_header("Пример 4: Специальные математические константы");

    // Золотое сечение φ
    ContinuedFraction phi = ContinuedFraction::from_double(1.618033988749895, 10);
    std::cout << "1. Золотое сечение φ: " << phi << std::endl;
    std::cout << "   Значение: " << phi.to_double() << std::endl;

    // √2
    ContinuedFraction sqrt2 = sqrt_continued_fraction(2, 10);
    std::cout << "\n2. √2: " << sqrt2 << std::endl;
    std::cout << "   Значение: " << sqrt2.to_double() << std::endl;
    std::cout << "   Проверка: " << sqrt2.to_double() * sqrt2.to_double() << std::endl;

    // e
    ContinuedFraction e = e_continued_fraction(10);
    std::cout << "\n3. Число e: " << e << std::endl;
    std::cout << "   Значение: " << e.to_double() << std::endl;

    // √3 (периодическая дробь)
    ContinuedFraction sqrt3 = sqrt_continued_fraction(3, 10);
    std::cout << "\n4. √3: " << sqrt3 << std::endl;
    std::cout << "   Значение: " << sqrt3.to_double() << std::endl;
}

/**
 * @brief Пример 5: Периодические цепные дроби
 */
void example_periodic() {
    print_header("Пример 5: Периодические цепные дроби");

    // Периодическая дробь для √13
    ContinuedFraction sqrt13 = sqrt_continued_fraction(13, 15);
    std::cout << "√13 как цепная дробь: " << sqrt13 << std::endl;
    std::cout << "Значение: " << sqrt13.to_double() << std::endl;
    std::cout << "Квадрат: " << sqrt13.to_double() * sqrt13.to_double() << std::endl;

    // Проверка периодичности
    std::cout << "\nСвойства:" << std::endl;
    std::cout << "Периодическая: " << (sqrt13.is_periodic() ? "да" : "нет") << std::endl;
    std::cout << "Конечная: " << (sqrt13.is_finite() ? "да" : "нет") << std::endl;
    std::cout << "Количество коэффициентов: " << sqrt13.size() << std::endl;
}

/**
 * @brief Пример 6: Операторы сравнения
 */
void example_comparison() {
    print_header("Пример 6: Операторы сравнения");

    ContinuedFraction a("[1; 2, 3]");
    ContinuedFraction b("[1; 2, 4]");
    ContinuedFraction c = a;

    std::cout << "a = " << a << " ≈ " << a.to_double() << std::endl;
    std::cout << "b = " << b << " ≈ " << b.to_double() << std::endl;
    std::cout << "c = " << c << " ≈ " << c.to_double() << std::endl;

    std::cout << "\nСравнения:" << std::endl;
    std::cout << "a == b: " << (a == b ? "true" : "false") << std::endl;
    std::cout << "a == c: " << (a == c ? "true" : "false") << std::endl;
    std::cout << "a != b: " << (a != b ? "true" : "false") << std::endl;
    std::cout << "a < b: " << (a < b ? "true" : "false") << std::endl;
    std::cout << "a > b: " << (a > b ? "true" : "false") << std::endl;
    std::cout << "a <= b: " << (a <= b ? "true" : "false") << std::endl;
    std::cout << "a >= b: " << (a >= b ? "true" : "false") << std::endl;
}

/**
 * @brief Главная функция программы
 */
int main() {
    //UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // locale::global(locale(""));
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "ДЕМОНСТРАЦИЯ БИБЛИОТЕКИ ЦЕПНЫХ ДРОБЕЙ\n";
    std::cout << "======================================\n";

    try {
        // Запуск всех примеров
        example_creation();
        example_operations();
        example_convergents();
        example_special_numbers();
        example_periodic();
        example_comparison();

        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "  Все примеры выполнены успешно!\n";
        std::cout << std::string(60, '=') << "\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\nОШИБКА: " << e.what() << std::endl;
        return 1;
    }
}