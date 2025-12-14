/**
* @file continued_fraction.h
 * @author КИЗАИДИ АНТОНИУ
 * @date 05/12/2025
 * @brief Заголовочный файл библиотеки для работы с цепными дробями
 *
 * Реализация алгебраического объекта "Цепная дробь" с поддержкой
 * символьных вычислений, упрощений и базовых алгебраических операций.
 *
 * Цепная дробь - математическое выражение вида:
 * a₀ + 1/(a₁ + 1/(a₂ + 1/(a₃ + ...)))
 * где aᵢ - целые коэффициенты.
 *
 * Лицензия: MIT
 */

#ifndef CONTINUED_FRACTION_H
#define CONTINUED_FRACTION_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <type_traits>
#include <memory>
#include <limits>
#include <regex>

namespace Math {
    /**
     * @class ContinuedFraction
     * @brief Класс для представления цепных дробей
     *
     * Поддерживает конечные и периодические цепные дроби,
     * алгебраические операции, преобразования и вычисления.
     */
    class ContinuedFraction {
    private:
        /**
         * @struct Coefficient
         * @brief Внутренняя структура для хранения коэффициентов с метаданными
         */
        struct Coefficient {
            long long value;           ///< Абсолютное значение коэффициента
            bool is_negative;          ///< Флаг отрицательности
            bool is_periodic_marker;   ///< Флаг начала периодической части

            /**
             * @brief Конструктор коэффициента
             * @param val Значение коэффициента
             * @param neg Флаг отрицательности
             * @param periodic Флаг начала периода
             */
            explicit Coefficient(long long val = 0,
                               bool neg = false,
                               bool periodic = false)
                : value(std::abs(val))
                , is_negative(val < 0 || neg)
                , is_periodic_marker(periodic) {}

            /**
             * @brief Получить знаковое значение коэффициента
             * @return Значение с учетом знака
             */
            long long signed_value() const {
                return is_negative ? -value : value;
            }

            /**
             * @brief Оператор сравнения коэффициентов
             */
            bool operator==(const Coefficient& other) const {
                return signed_value() == other.signed_value() &&
                       is_periodic_marker == other.is_periodic_marker;
            }
        };

        // Приватные поля класса
        std::vector<Coefficient> coefficients_;  ///< Вектор коэффициентов цепной дроби
        bool is_finite_;                         ///< Флаг конечности дроби
        bool is_periodic_;                       ///< Флаг периодичности
        mutable double cached_value_;            ///< Кэшированное числовое значение
        mutable bool value_cached_;              ///< Флаг валидности кэша

        /**
         * @brief Нормализация коэффициентов цепной дроби
         *
         * Удаляет лишние нули, выполняет оптимизацию вида [..., a, 1, b] → [..., a+1, b]
         */
        void normalize();

        /**
         * @brief Обновление флагов конечности и периодичности
         */
        void update_flags();

        /**
         * @brief Инвалидация кэшированного значения
         */
        void invalidate_cache() const;

        /**
         * @brief Вычисление n-й подходящей дроби
         * @param n Индекс подходящей дроби
         * @return Пара {числитель, знаменатель}
         */
        std::pair<long long, long long> compute_convergent(size_t n) const;

    public:
        // ==================== КОНСТРУКТОРЫ И ДЕСТРУКТОР ====================

        /**
         * @brief Конструктор по умолчанию
         *
         * Создает цепную дробь [0]
         */
        ContinuedFraction();

        /**
         * @brief Конструктор из целого числа
         * @param value Целое число
         */
        explicit ContinuedFraction(long long value);

        /**
         * @brief Конструктор из вектора коэффициентов
         * @param coeffs Вектор целых коэффициентов
         */
        explicit ContinuedFraction(const std::vector<long long>& coeffs);

        /**
         * @brief Конструктор из строкового представления
         * @param str Строка в формате "[a0; a1, a2, ...]"
         * @throw std::invalid_argument При неверном формате строки
         */
        explicit ContinuedFraction(const std::string& str);

        /**
         * @brief Конструктор копирования
         */
        ContinuedFraction(const ContinuedFraction& other) = default;

        /**
         * @brief Конструктор перемещения
         */
        ContinuedFraction(ContinuedFraction&& other) noexcept = default;

        /**
         * @brief Деструктор
         */
        ~ContinuedFraction() = default;

        // ==================== ОПЕРАТОРЫ ПРИСВАИВАНИЯ ====================

        /**
         * @brief Оператор копирующего присваивания
         */
        ContinuedFraction& operator=(const ContinuedFraction& other) = default;

        /**
         * @brief Оператор перемещающего присваивания
         */
        ContinuedFraction& operator=(ContinuedFraction&& other) noexcept = default;

        // ==================== ОСНОВНЫЕ МЕТОДЫ ====================

        /**
         * @brief Получить коэффициенты цепной дроби
         * @return Вектор коэффициентов
         */
        std::vector<long long> get_coefficients() const;

        /**
         * @brief Установить новые коэффициенты
         * @param coeffs Вектор новых коэффициентов
         */
        void set_coefficients(const std::vector<long long>& coeffs);

        /**
         * @brief Добавить коэффициент в конец цепной дроби
         * @param coeff Новый коэффициент
         */
        void add_coefficient(long long coeff);

        /**
         * @brief Преобразовать цепную дробь в числовое значение
         * @return Приближенное значение типа double
         */
        double to_double() const;

        /**
         * @brief Получить n-ю подходящую дробь
         * @param n Индекс подходящей дроби (0-based)
         * @return Пара {числитель, знаменатель}
         * @throw std::out_of_range При выходе за границы
         */
        std::pair<long long, long long> convergent(size_t n) const;

        /**
         * @brief Упростить цепную дробь
         *
         * Выполняет нормализацию и оптимизацию коэффициентов
         */
        void simplify();

        // ==================== АРИФМЕТИЧЕСКИЕ ОПЕРАТОРЫ ====================

        /**
         * @brief Оператор сложения
         */
        ContinuedFraction operator+(const ContinuedFraction& other) const;

        /**
         * @brief Оператор вычитания
         */
        ContinuedFraction operator-(const ContinuedFraction& other) const;

        /**
         * @brief Оператор умножения
         */
        ContinuedFraction operator*(const ContinuedFraction& other) const;

        /**
         * @brief Оператор деления
         * @throw std::runtime_error При делении на ноль
         */
        ContinuedFraction operator/(const ContinuedFraction& other) const;

        /**
         * @brief Составной оператор сложения с присваиванием
         */
        ContinuedFraction& operator+=(const ContinuedFraction& other);

        /**
         * @brief Составной оператор вычитания с присваиванием
         */
        ContinuedFraction& operator-=(const ContinuedFraction& other);

        /**
         * @brief Составной оператор умножения с присваиванием
         */
        ContinuedFraction& operator*=(const ContinuedFraction& other);

        /**
         * @brief Составной оператор деления с присваиванием
         */
        ContinuedFraction& operator/=(const ContinuedFraction& other);

        // ==================== ОПЕРАТОРЫ СРАВНЕНИЯ ====================

        /**
         * @brief Оператор равенства
         *
         * Сравнивает точное представление цепных дробей
         */
        bool operator==(const ContinuedFraction& other) const;

        /**
         * @brief Оператор неравенства
         */
        bool operator!=(const ContinuedFraction& other) const;

        /**
         * @brief Оператор меньше
         *
         * Сравнивает числовые значения
         */
        bool operator<(const ContinuedFraction& other) const;

        /**
         * @brief Оператор меньше или равно
         */
        bool operator<=(const ContinuedFraction& other) const;

        /**
         * @brief Оператор больше
         */
        bool operator>(const ContinuedFraction& other) const;

        /**
         * @brief Оператор больше или равно
         */
        bool operator>=(const ContinuedFraction& other) const;

        // ==================== ВВОД/ВЫВОД ====================

        /**
         * @brief Преобразовать в строковое представление
         * @return Строка в формате "[a0; a1, a2, ...]"
         */
        std::string to_string() const;

        /**
         * @brief Парсинг строкового представления
         * @param str Строка для парсинга
         * @throw std::invalid_argument При неверном формате
         */
        void parse_string(const std::string& str);

        /**
         * @brief Оператор вывода в поток
         */
        friend std::ostream& operator<<(std::ostream& os, const ContinuedFraction& cf);

        /**
         * @brief Оператор ввода из потока
         */
        friend std::istream& operator>>(std::istream& is, ContinuedFraction& cf);

        // ==================== СТАТИЧЕСКИЕ МЕТОДЫ ====================

        /**
         * @brief Создать цепную дробь из десятичного числа
         * @param value Десятичное число
         * @param max_terms Максимальное количество коэффициентов
         * @return Цепная дробь
         */
        static ContinuedFraction from_double(double value, size_t max_terms = 20);

        /**
         * @brief Создать цепную дробь из рационального числа
         * @param numerator Числитель
         * @param denominator Знаменатель
         * @return Цепная дробь
         * @throw std::invalid_argument При нулевом знаменателе
         */
        static ContinuedFraction from_rational(long long numerator, long long denominator);

        /**
         * @brief Создать периодическую цепную дробь
         * @param non_periodic Непериодическая часть
         * @param periodic Периодическая часть
         * @return Периодическая цепная дробь
         */
        static ContinuedFraction create_periodic(
            const std::vector<long long>& non_periodic,
            const std::vector<long long>& periodic);

        // ==================== СВОЙСТВА ====================

        /**
         * @brief Проверить конечность дроби
         */
        bool is_finite() const;

        /**
         * @brief Проверить периодичность дроби
         */
        bool is_periodic() const;

        /**
         * @brief Получить количество коэффициентов
         */
        size_t size() const;

        /**
         * @brief Проверить, является ли дробь целым числом
         */
        bool is_integer() const;

        /**
         * @brief Очистить цепную дробь
         *
         * Устанавливает дробь в состояние [0]
         */
        void clear();
    };

    // ==================== ВНЕШНИЕ ФУНКЦИИ ====================

    /**
     * @brief Вычислить цепную дробь для квадратного корня
     * @param n Число, из которого извлекается корень
     * @param max_terms Максимальное количество коэффициентов
     * @return Цепная дробь для √n
     * @throw std::invalid_argument При отрицательном n
     */
    ContinuedFraction sqrt_continued_fraction(long long n, size_t max_terms = 20);

    /**
     * @brief Вычислить цепную дробь для числа e
     * @param max_terms Максимальное количество коэффициентов
     * @return Цепная дробь для e
     */
    ContinuedFraction e_continued_fraction(size_t max_terms = 20);

    /**
     * @brief Вычислить цепную дробь для числа π
     * @param max_terms Максимальное количество коэффициентов
     * @return Приближение π цепной дробью
     */
    ContinuedFraction pi_continued_fraction(size_t max_terms = 20);

    // ==================== INLINE-ФУНКЦИИ ====================

    /**
     * @brief Вычислить НОД двух чисел
     * @param a Первое число
     * @param b Второе число
     * @return Наибольший общий делитель
     */
    inline long long gcd(long long a, long long b) {
        while (b != 0) {
            long long temp = b;
            b = a % b;
            a = temp;
        }
        return std::abs(a);
    }

    /**
     * @brief Приблизительное сравнение цепных дробей
     * @param a Первая дробь
     * @param b Вторая дробь
     * @param epsilon Точность сравнения
     * @return true, если значения отличаются меньше чем на epsilon
     */
    inline bool approximately_equal(const ContinuedFraction& a,
                                  const ContinuedFraction& b,
                                  double epsilon = 1e-12) {
        return std::abs(a.to_double() - b.to_double()) < epsilon;
    }
}

#endif // CONTINUED_FRACTION_H