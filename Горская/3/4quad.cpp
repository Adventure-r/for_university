#include <iostream>
#include <cmath>
#include <utility>

// Функция, минимум которой мы ищем
double function(double x) {
    return std::exp(x) + std::exp(-3 * x) - 4;
}

// Функция обратной квадратичной интерполяции для поиска минимума
std::pair<double, int> inverseQuadraticInterpolation(double x0, double x1, double x2, double tolerance, int maxIterations) {
    // Начальные значения функции в точках x0, x1, x2
    double f0 = function(x0);
    double f1 = function(x1);
    double f2 = function(x2);

    int i;
    
    // Основной цикл итераций
    for (i = 0; i < maxIterations; ++i) {
        // Проверка, чтобы избежать деления на ноль
        if (std::abs(f0 - f1) < 1e-10 || std::abs(f1 - f2) < 1e-10 || std::abs(f2 - f0) < 1e-10) {
            // Если значения функции слишком близки, возвращаем среднюю точку
            return {(x0 + x1 + x2) / 3.0, i};
        }

        // Вычисление новой точки с помощью формулы обратной квадратичной интерполяции
        // Формула: x = x0 * (f1*f2) / ((f0-f1)*(f0-f2)) + x1 * (f0*f2) / ((f1-f0)*(f1-f2)) + x2 * (f0*f1) / ((f2-f0)*(f2-f1))
        double term1 = x0 * f1 * f2 / ((f0 - f1) * (f0 - f2));
        double term2 = x1 * f0 * f2 / ((f1 - f0) * (f1 - f2));
        double term3 = x2 * f0 * f1 / ((f2 - f0) * (f2 - f1));
        double x_new = term1 + term2 + term3;

        // Вычисление значения функции в новой точке
        double f_new = function(x_new);

        // Проверка условия сходимости: если разница между x_new и одной из предыдущих точек мала
        if (std::abs(x_new - x0) < tolerance || std::abs(x_new - x1) < tolerance || std::abs(x_new - x2) < tolerance) {
            return {x_new, i}; // Возвращаем найденную точку как минимум
        }

        // Обновление точек: выбираем три точки с наименьшими значениями функции
        // Сравниваем f_new с текущими значениями f0, f1, f2
        if (f_new < f1) {
            // Если f_new меньше f1, заменяем точку с наибольшим значением функции
            if (f0 > f1 && f0 > f2) {
                x0 = x_new;
                f0 = f_new;
            } else if (f2 > f0 && f2 > f1) {
                x2 = x_new;
                f2 = f_new;
            } else {
                x1 = x_new;
                f1 = f_new;
            }
        } else {
            // Если f_new не улучшает минимум, заменяем точку с наибольшим значением функции
            if (f0 > f1 && f0 > f2) {
                x0 = x_new;
                f0 = f_new;
            } else if (f2 > f0 && f2 > f1) {
                x2 = x_new;
                f2 = f_new;
            } else {
                x1 = x_new;
                f1 = f_new;
            }
        }
    }

    // Если достигнуто максимальное количество итераций, возвращаем среднюю точку
    return {(x0 + x1 + x2) / 3.0, i};
}

int main() {
    // Начальные точки для интерполяции
    double x0 = -1.0;
    double x1 = 0.0;
    double x2 = 1.0;

    // Точность и максимальное количество итераций
    double tolerance = 1e-6;
    int maxIterations = 100000;

    // Вызов функции обратной квадратичной интерполяции
    auto [minimum, iter] = inverseQuadraticInterpolation(x0, x1, x2, tolerance, maxIterations);

    // Вывод результата
    std::cout << "Найденный минимум в точке x = " << minimum << std::endl;
    std::cout << "Значение функции в минимуме: f(x) = " << function(minimum) << std::endl;
    std::cout << "Кол-во понадобившихся итераций= " << iter << std::endl;
    return 0;
}
