/**
* @file continued_fraction.cpp
 * @brief Реализация методов класса ContinuedFraction
 * @author КИЗАИДИ АНТОНИУ
 * @date 05/12/2025
 * @brief Реализация методов класса ContinuedFraction
 * Реализация всех методов, объявленных в continued_fraction.h.
 * Включает алгоритмы Евклида, вычисление подходящих дробей,
 * операции с цепными дробями и специальные функции.
 */

#include "continued_fraction.h"
#include <algorithm>
#include <functional>

namespace Math {
    // ==================== РЕАЛИЗАЦИЯ ПРИВАТНЫХ МЕТОДОВ ====================

    /**
     * @brief Нормализация коэффициентов цепной дроби
     *
     * Выполняет:
     * 1. Удаление нулевых коэффициентов (кроме первого)
     * 2. Оптимизацию вида [..., a, 1, b] → [..., a+1, b]
     * 3. Обновление флагов состояния
     */
    void ContinuedFraction::normalize() {
        // Удаление нулевых коэффициентов, кроме первого
        if (coefficients_.size() > 1) {
            auto it = std::remove_if(coefficients_.begin() + 1,
                                    coefficients_.end(),
                                    [](const Coefficient& c) {
                                        return c.value == 0;
                                    });
            coefficients_.erase(it, coefficients_.end());
        }

        // Оптимизация: [..., a, 1, b] → [..., a+1, b]
        for (size_t i = 0; i + 2 < coefficients_.size(); ++i) {
            if (coefficients_[i + 1].value == 1 &&
                !coefficients_[i + 1].is_negative &&
                !coefficients_[i + 1].is_periodic_marker) {

                // Вычисляем новый коэффициент
                long long new_val = coefficients_[i].signed_value() +
                                  coefficients_[i + 2].signed_value();
                coefficients_[i] = Coefficient(new_val);

                // Удаляем коэффициенты 1 и b
                coefficients_.erase(coefficients_.begin() + i + 1,
                                   coefficients_.begin() + i + 3);
                --i; // Корректируем индекс после удаления
            }
        }

        // Обновление состояния
        update_flags();
        invalidate_cache();
    }

    /**
     * @brief Обновление флагов состояния
     *
     * Определяет, является ли дробь периодической и конечной
     */
    void ContinuedFraction::update_flags() {
        // Проверяем наличие маркера начала периода
        is_periodic_ = std::any_of(coefficients_.begin(),
                                  coefficients_.end(),
                                  [](const Coefficient& c) {
                                      return c.is_periodic_marker;
                                  });

        // Если есть период, дробь бесконечна
        is_finite_ = !is_periodic_;
    }

    /**
     * @brief Инвалидация кэшированного значения
     *
     * Вызывается при изменении коэффициентов
     */
    void ContinuedFraction::invalidate_cache() const {
        value_cached_ = false;
        cached_value_ = 0.0;
    }

    /**
     * @brief Вычисление n-й подходящей дроби
     * @param n Индекс подходящей дроби
     * @return Пару {числитель, знаменатель}
     *
     * Использует рекуррентные формулы:
     * p₋₁ = 1, p₀ = a₀, pᵢ = aᵢ·pᵢ₋₁ + pᵢ₋₂
     * q₋₁ = 0, q₀ = 1, qᵢ = aᵢ·qᵢ₋₁ + qᵢ₋₂
     */
    std::pair<long long, long long> ContinuedFraction::compute_convergent(size_t n) const {
        // Базовый случай: нулевая подходящая дробь
        if (n == 0) {
            return {coefficients_[0].signed_value(), 1};
        }

        // Инициализация по формулам
        long long prev_num = coefficients_[0].signed_value();
        long long prev_den = 1;
        long long curr_num = coefficients_[0].signed_value() *
                           coefficients_[1].signed_value() + 1;
        long long curr_den = coefficients_[1].signed_value();

        // Рекуррентное вычисление
        for (size_t i = 2; i <= n; ++i) {
            long long coeff = coefficients_[i % coefficients_.size()].signed_value();
            long long new_num = coeff * curr_num + prev_num;
            long long new_den = coeff * curr_den + prev_den;

            prev_num = curr_num;
            prev_den = curr_den;
            curr_num = new_num;
            curr_den = new_den;
        }

        return {curr_num, curr_den};
    }

    // ==================== РЕАЛИЗАЦИЯ КОНСТРУКТОРОВ ====================

    /**
     * @brief Конструктор по умолчанию
     *
     * Создает цепную дробь [0]
     */
    ContinuedFraction::ContinuedFraction()
        : coefficients_{Coefficient(0)}
        , is_finite_(true)
        , is_periodic_(false)
        , cached_value_(0.0)
        , value_cached_(false) {}

    /**
     * @brief Конструктор из целого числа
     * @param value Целое число
     */
    ContinuedFraction::ContinuedFraction(long long value)
        : coefficients_{Coefficient(value)}
        , is_finite_(true)
        , is_periodic_(false)
        , cached_value_(static_cast<double>(value))
        , value_cached_(true) {}

    /**
     * @brief Конструктор из вектора коэффициентов
     * @param coeffs Вектор коэффициентов
     */
    ContinuedFraction::ContinuedFraction(const std::vector<long long>& coeffs)
        : is_finite_(true)
        , is_periodic_(false)
        , cached_value_(0.0)
        , value_cached_(false) {

        coefficients_.reserve(coeffs.size());
        for (long long c : coeffs) {
            coefficients_.emplace_back(c);
        }
        normalize();
    }

    /**
     * @brief Конструктор из строки
     * @param str Строковое представление
     * @throw std::invalid_argument при неверном формате
     */
    ContinuedFraction::ContinuedFraction(const std::string& str) {
        parse_string(str);
    }

    // ==================== РЕАЛИЗАЦИЯ ОСНОВНЫХ МЕТОДОВ ====================

    /**
     * @brief Получить коэффициенты
     * @return Вектор коэффициентов со знаками
     */
    std::vector<long long> ContinuedFraction::get_coefficients() const {
        std::vector<long long> result;
        result.reserve(coefficients_.size());
        for (const auto& coeff : coefficients_) {
            result.push_back(coeff.signed_value());
        }
        return result;
    }

    /**
     * @brief Установить новые коэффициенты
     * @param coeffs Новые коэффициенты
     */
    void ContinuedFraction::set_coefficients(const std::vector<long long>& coeffs) {
        coefficients_.clear();
        coefficients_.reserve(coeffs.size());
        for (long long c : coeffs) {
            coefficients_.emplace_back(c);
        }
        normalize();
    }

    /**
     * @brief Добавить коэффициент
     * @param coeff Новый коэффициент
     */
    void ContinuedFraction::add_coefficient(long long coeff) {
        coefficients_.emplace_back(coeff);
        normalize();
    }

    /**
     * @brief Преобразовать в double
     * @return Числовое значение дроби
     *
     * Использует кэширование для оптимизации
     */
    double ContinuedFraction::to_double() const {
        if (value_cached_) {
            return cached_value_;
        }

        if (coefficients_.empty()) {
            cached_value_ = 0.0;
            value_cached_ = true;
            return cached_value_;
        }

        // Вычисление значения с конца для численной устойчивости
        double value = 0.0;
        for (auto it = coefficients_.rbegin(); it != coefficients_.rend(); ++it) {
            if (value == 0.0) {
                value = static_cast<double>(it->signed_value());
            } else {
                value = static_cast<double>(it->signed_value()) + 1.0 / value;
            }
        }

        // Кэширование результата
        cached_value_ = value;
        value_cached_ = true;
        return cached_value_;
    }

    /**
     * @brief Получить подходящую дробь
     * @param n Индекс подходящей дроби
     * @return Пару {числитель, знаменатель}
     * @throw std::out_of_range при недопустимом индексе
     */
    std::pair<long long, long long> ContinuedFraction::convergent(size_t n) const {
        if (n >= coefficients_.size() && is_finite_) {
            throw std::out_of_range("Индекс подходящей дроби вне диапазона");
        }
        return compute_convergent(n);
    }

    /**
     * @brief Упростить цепную дробь
     */
    void ContinuedFraction::simplify() {
        normalize();
    }

    // ==================== РЕАЛИЗАЦИЯ АРИФМЕТИЧЕСКИХ ОПЕРАТОРОВ ====================

    /**
     * @brief Сложение цепных дробей
     *
     * Внимание: текущая реализация через преобразование к double.
     * Для точной арифметики требуются специальные алгоритмы.
     */
    ContinuedFraction ContinuedFraction::operator+(const ContinuedFraction& other) const {
        double result = this->to_double() + other.to_double();
        return ContinuedFraction::from_double(result);
    }

    /**
     * @brief Вычитание цепных дробей
     */
    ContinuedFraction ContinuedFraction::operator-(const ContinuedFraction& other) const {
        double result = this->to_double() - other.to_double();
        return ContinuedFraction::from_double(result);
    }

    /**
     * @brief Умножение цепных дробей
     */
    ContinuedFraction ContinuedFraction::operator*(const ContinuedFraction& other) const {
        double result = this->to_double() * other.to_double();
        return ContinuedFraction::from_double(result);
    }

    /**
     * @brief Деление цепных дробей
     * @throw std::runtime_error при делении на ноль
     */
    ContinuedFraction ContinuedFraction::operator/(const ContinuedFraction& other) const {
        if (std::abs(other.to_double()) < 1e-15) {
            throw std::runtime_error("Деление на ноль");
        }
        double result = this->to_double() / other.to_double();
        return ContinuedFraction::from_double(result);
    }

    /**
     * @brief Составной оператор сложения
     */
    ContinuedFraction& ContinuedFraction::operator+=(const ContinuedFraction& other) {
        *this = *this + other;
        return *this;
    }

    /**
     * @brief Составной оператор вычитания
     */
    ContinuedFraction& ContinuedFraction::operator-=(const ContinuedFraction& other) {
        *this = *this - other;
        return *this;
    }

    /**
     * @brief Составной оператор умножения
     */
    ContinuedFraction& ContinuedFraction::operator*=(const ContinuedFraction& other) {
        *this = *this * other;
        return *this;
    }

    /**
     * @brief Составной оператор деления
     */
    ContinuedFraction& ContinuedFraction::operator/=(const ContinuedFraction& other) {
        *this = *this / other;
        return *this;
    }

    // ==================== РЕАЛИЗАЦИЯ ОПЕРАТОРОВ СРАВНЕНИЯ ====================

    /**
     * @brief Проверка точного равенства
     *
     * Сравнивает коэффициенты и их метаданные
     */
    bool ContinuedFraction::operator==(const ContinuedFraction& other) const {
        if (coefficients_.size() != other.coefficients_.size()) {
            return false;
        }

        for (size_t i = 0; i < coefficients_.size(); ++i) {
            if (!(coefficients_[i] == other.coefficients_[i])) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Проверка неравенства
     */
    bool ContinuedFraction::operator!=(const ContinuedFraction& other) const {
        return !(*this == other);
    }

    /**
     * @brief Оператор "меньше"
     *
     * Сравнивает численные значения
     */
    bool ContinuedFraction::operator<(const ContinuedFraction& other) const {
        return this->to_double() < other.to_double();
    }

    /**
     * @brief Оператор "меньше или равно"
     */
    bool ContinuedFraction::operator<=(const ContinuedFraction& other) const {
        return this->to_double() <= other.to_double();
    }

    /**
     * @brief Оператор "больше"
     */
    bool ContinuedFraction::operator>(const ContinuedFraction& other) const {
        return this->to_double() > other.to_double();
    }

    /**
     * @brief Оператор "больше или равно"
     */
    bool ContinuedFraction::operator>=(const ContinuedFraction& other) const {
        return this->to_double() >= other.to_double();
    }

    // ==================== РЕАЛИЗАЦИЯ ВВОДА/ВЫВОДА ====================

    /**
     * @brief Преобразование в строку
     * @return Строковое представление
     *
     * Формат: [a0; a1, a2, ...] для конечных дробей
     * Формат: [a0; (a1, a2, ...)] для периодических
     */
    std::string ContinuedFraction::to_string() const {
        if (coefficients_.empty()) {
            return "[0]";
        }

        std::ostringstream oss;
        oss << "[" << coefficients_[0].signed_value();

        for (size_t i = 1; i < coefficients_.size(); ++i) {
            if (coefficients_[i].is_periodic_marker) {
                oss << "; (";
            } else {
                oss << "; ";
            }
            oss << coefficients_[i].signed_value();
        }

        if (is_periodic_) {
            oss << ")]";
        } else {
            oss << "]";
        }

        return oss.str();
    }

    /**
     * @brief Парсинг строки
     * @param str Строка для парсинга
     * @throw std::invalid_argument при ошибке
     *
     * Поддерживает форматы:
     * - [a0; a1, a2, ...]
     * - [a0; (a1, a2, ...)]
     */
    void ContinuedFraction::parse_string(const std::string& str) {
        coefficients_.clear();

        // Регулярное выражение для проверки формата
        std::regex pattern(R"(^\[([^\[\]]+)\]$)");
        std::smatch match;

        if (!std::regex_match(str, match, pattern)) {
            throw std::invalid_argument("Неверный формат цепной дроби");
        }

        std::string content = match[1];
        std::istringstream iss(content);
        char ch;

        // Чтение первого коэффициента
        long long coeff;
        if (!(iss >> coeff)) {
            throw std::invalid_argument("Ошибка чтения первого коэффициента");
        }
        coefficients_.emplace_back(coeff);

        // Чтение остальных коэффициентов
        while (iss >> ch && ch == ';') {
            iss >> coeff;
            coefficients_.emplace_back(coeff);
        }

        normalize();
    }

    /**
     * @brief Оператор вывода
     */
    std::ostream& operator<<(std::ostream& os, const ContinuedFraction& cf) {
        os << cf.to_string();
        return os;
    }

    /**
     * @brief Оператор ввода
     */
    std::istream& operator>>(std::istream& is, ContinuedFraction& cf) {
        std::string str;
        std::getline(is, str);
        cf.parse_string(str);
        return is;
    }

    // ==================== РЕАЛИЗАЦИЯ СТАТИЧЕСКИХ МЕТОДОВ ====================

    /**
     * @brief Создание из десятичного числа
     * @param value Десятичное число
     * @param max_terms Максимальное число коэффициентов
     * @return Цепная дробь
     *
     * Алгоритм: повторное выделение целой части
     */
    ContinuedFraction ContinuedFraction::from_double(double value, size_t max_terms) {
        std::vector<long long> coeffs;

        for (size_t i = 0; i < max_terms; ++i) {
            long long integer_part = static_cast<long long>(std::floor(value));
            coeffs.push_back(integer_part);

            double fractional = value - integer_part;
            if (std::abs(fractional) < 1e-12) {
                break;
            }

            value = 1.0 / fractional;
        }

        return ContinuedFraction(coeffs);
    }

    /**
     * @brief Создание из рационального числа
     * @param numerator Числитель
     * @param denominator Знаменатель
     * @return Цепная дробь
     * @throw std::invalid_argument при нулевом знаменателе
     *
     * Использует алгоритм Евклида
     */
    ContinuedFraction ContinuedFraction::from_rational(long long numerator,
                                                      long long denominator) {
        if (denominator == 0) {
            throw std::invalid_argument("Знаменатель не может быть нулевым");
        }

        std::vector<long long> coeffs;
        long long n = numerator;
        long long d = denominator;

        // Алгоритм Евклида для цепных дробей
        while (d != 0) {
            long long q = n / d;
            long long r = n % d;
            coeffs.push_back(q);
            n = d;
            d = r;
        }

        return ContinuedFraction(coeffs);
    }

    /**
     * @brief Создание периодической дроби
     * @param non_periodic Непериодическая часть
     * @param periodic Периодическая часть
     * @return Периодическая цепная дробь
     */
    ContinuedFraction ContinuedFraction::create_periodic(
        const std::vector<long long>& non_periodic,
        const std::vector<long long>& periodic) {

        std::vector<Coefficient> coeffs;

        // Непериодическая часть
        for (long long c : non_periodic) {
            coeffs.emplace_back(c);
        }

        // Периодическая часть с маркером
        if (!periodic.empty()) {
            coeffs.emplace_back(periodic[0], false, true);
            for (size_t i = 1; i < periodic.size(); ++i) {
                coeffs.emplace_back(periodic[i]);
            }
        }

        ContinuedFraction cf;
        cf.coefficients_ = coeffs;
        cf.normalize();
        return cf;
    }

    // ==================== РЕАЛИЗАЦИЯ СВОЙСТВ ====================

    /**
     * @brief Проверка конечности
     */
    bool ContinuedFraction::is_finite() const { return is_finite_; }

    /**
     * @brief Проверка периодичности
     */
    bool ContinuedFraction::is_periodic() const { return is_periodic_; }

    /**
     * @brief Количество коэффициентов
     */
    size_t ContinuedFraction::size() const { return coefficients_.size(); }

    /**
     * @brief Проверка целочисленности
     */
    bool ContinuedFraction::is_integer() const {
        return coefficients_.size() == 1;
    }

    /**
     * @brief Очистка дроби
     */
    void ContinuedFraction::clear() {
        coefficients_.clear();
        coefficients_.emplace_back(0);
        normalize();
    }

    // ==================== РЕАЛИЗАЦИЯ ВНЕШНИХ ФУНКЦИЙ ====================

    /**
     * @brief Цепная дробь для квадратного корня
     * @param n Число под корнем
     * @param max_terms Максимальное число коэффициентов
     * @return Цепная дробь √n
     * @throw std::invalid_argument при отрицательном n
     *
     * Алгоритм: метод последовательных приближений
     * Для √n цепная дробь всегда периодическая
     */
    ContinuedFraction sqrt_continued_fraction(long long n, size_t max_terms) {
        if (n < 0) {
            throw std::invalid_argument("Нельзя вычислить корень из отрицательного числа");
        }

        std::vector<long long> coeffs;
        long long m = 0;
        long long d = 1;
        long long a0 = static_cast<long long>(std::sqrt(n));

        // Если n - полный квадрат
        if (a0 * a0 == n) {
            return ContinuedFraction(a0);
        }

        coeffs.push_back(a0);
        long long a = a0;

        // Вычисление периода
        for (size_t i = 0; i < max_terms; ++i) {
            m = d * a - m;
            d = (n - m * m) / d;
            a = (a0 + m) / d;
            coeffs.push_back(a);

            // Критерий завершения периода
            if (a == 2 * a0) {
                break;
            }
        }

        // Создание периодической дроби
        return ContinuedFraction::create_periodic({a0},
            std::vector<long long>(coeffs.begin() + 1, coeffs.end()));
    }

    /**
     * @brief Цепная дробь для числа e
     * @param max_terms Максимальное число коэффициентов
     * @return Цепная дробь e
     *
     * Известное разложение: e = [2; 1, 2, 1, 1, 4, 1, 1, 6, 1, ...]
     */
    ContinuedFraction e_continued_fraction(size_t max_terms) {
        std::vector<long long> coeffs;
        coeffs.push_back(2);  // Первый коэффициент

        for (size_t i = 1; i < max_terms; ++i) {
            if (i % 3 == 2) {
                // Каждый третий коэффициент, начиная со второго
                coeffs.push_back(2 * ((i + 1) / 3));
            } else {
                // Остальные коэффициенты равны 1
                coeffs.push_back(1);
            }
        }

        return ContinuedFraction(coeffs);
    }

    /**
     * @brief Цепная дробь для числа π
     * @param max_terms Максимальное число коэффициентов
     * @return Приближение π
     *
     * Внимание: это простая цепная дробь, не оптимальное приближение
     * Для лучших приближений нужны специальные алгоритмы
     */
    ContinuedFraction pi_continued_fraction(size_t max_terms) {
        return ContinuedFraction::from_double(3.14159265358979323846, max_terms);
    }
}