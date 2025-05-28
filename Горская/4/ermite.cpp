#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

/*    Ввод данных – набор точек x, их значений y, и производных dydx.

    Создание сегментов – каждая пара соседних точек образует сегмент, включающий значения и их производные.

    Использование Эрмитовых базисных функций – вычисляем коэффициенты, чтобы учесть как значения, так и наклон (касательную).

    Вычисление интерполированных значений – находим новые значения между известными точками.

*/

// Структура для хранения данных интерполяции
struct HermiteSplineSegment {
    double x0, x1;  // Границы интервала
    double y0, y1;  // Значения функции на концах интервала
    double dydx0, dydx1;  // Значения производных на концах интервала
};

// Функция для вычисления гиперболического косинуса ch(x) = (e^x + e^(-x)) / 2
double ch(double x) {
    return (std::exp(x) + std::exp(-x)) / 2.0;
}

// Функция для вычисления гиперболического синуса sh(x) = (e^x - e^(-x)) / 2
double sh(double x) {
    return (std::exp(x) - std::exp(-x)) / 2.0;
}

// Функция для интерполяции Эрмита на заданном интервале
double hermiteSpline(const HermiteSplineSegment& segment, double x) {
    double h = segment.x1 - segment.x0; // Длина интервала
    double t = (x - segment.x0) / h;    // Нормализованная координата
    double t2 = t * t;
    double t3 = t2 * t;

    // Базисные функции Эрмита
    double h00 = 2 * t3 - 3 * t2 + 1;  // Коэффициент для y0
    double h10 = t3 - 2 * t2 + t;      // Коэффициент для dydx0 * h
    double h01 = -2 * t3 + 3 * t2;     // Коэффициент для y1
    double h11 = t3 - t2;              // Коэффициент для dydx1 * h

    // Вычисление интерполированного значения
    return h00 * segment.y0 +
           h10 * h * segment.dydx0 +
           h01 * segment.y1 +
           h11 * h * segment.dydx1;
}

// Основная программа
int main() {
    setlocale(LC_ALL, "ru");

    // Опорные точки x
    std::vector<double> x = { 1.00, 1.04, 1.08, 1.12, 1.16, 1.20 };
    std::vector<double> y, dydx; // Массивы значений функции и её производных

    // Вычисление ch(x) и sh(x) для заданных точек
    for (double xi : x) {
        y.push_back(ch(xi));      // Записываем ch(x) в массив y
        dydx.push_back(sh(xi));   // Записываем sh(x) в массив dydx
    }

    // Создание сегментов Эрмита (интервалов между точками)
    std::vector<HermiteSplineSegment> segments;
    for (size_t i = 0; i < x.size() - 1; ++i) {
        HermiteSplineSegment segment = { x[i], x[i + 1], y[i], y[i + 1], dydx[i], dydx[i + 1] };
        segments.push_back(segment);
    }

    // Точки, где нужно интерполировать значение функции
    std::vector<double> points = { 1.05, 1.09, 1.13, 1.15, 1.17 };
    std::cout << std::fixed << std::setprecision(6);

    // Интерполяция и вывод результатов
    std::cout << "Интерполированные значения:\n";
    for (double p : points) {
        // Найти соответствующий сегмент
        for (const auto& segment : segments) {
            if (p >= segment.x0 && p <= segment.x1) {
                double result = hermiteSpline(segment, p);
                std::cout << "H(" << p << ") = " << result << "\n";
                break;
            }
        }
    }

    return 0;
}
