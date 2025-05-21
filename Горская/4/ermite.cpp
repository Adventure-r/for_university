// сплайн эрмита

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

// Структура для хранения данных интерполяции
struct HermiteSplineSegment {
    double x0, x1;  // Границы интервала
    double y0, y1;  // Значения функции на концах интервала
    double dydx0, dydx1;  // Значения производных на концах интервала
};

/*
 cout << "e^x = " << exp(x) << endl;
    cout << "e^(-x) = " << exp(-x) << endl;
    cout << "sh(x) = " << sinh(x) << endl;
    cout << "ch(x) = " << cosh(x) << endl;
    cout << "sin(x) = " << sin(x) << endl;
    cout << "cos(x) = " << cos(x) << endl;
    cout << "ln(x) = " << log(x) << endl;
*/

// Функция для вычисления ch(x)
double ch(double x) {
    return (std::exp(x) + std::exp(-x)) / 2.0;
}

// Функция для вычисления производной ch'(x) = sh(x)
double sh(double x) {
    return (std::exp(x) - std::exp(-x)) / 2.0;
}

// Эрмитов сплайн
double hermiteSpline(const HermiteSplineSegment& segment, double x) {
    double h = segment.x1 - segment.x0; // Длина интервала
    double t = (x - segment.x0) / h;    // Нормализованная координата
    double t2 = t * t;
    double t3 = t2 * t;

    // Базисные функции Эрмита
    double h00 = 2 * t3 - 3 * t2 + 1;
    double h10 = t3 - 2 * t2 + t;
    double h01 = -2 * t3 + 3 * t2;
    double h11 = t3 - t2;

    // Вычисление значения сплайна
    return h00 * segment.y0 +
        h10 * h * segment.dydx0 +
        h01 * segment.y1 +
        h11 * h * segment.dydx1;
}

// Основная программа
int main() {
    setlocale(LC_ALL, "ru");
    std::vector<double> x = { 1.00, 1.04, 1.08, 1.12, 1.16, 1.20 };
    std::vector<double> y, dydx;

    // Вычисление значений ch(x) и производных sh(x)
    for (double xi : x) {
        y.push_back(ch(xi));
        dydx.push_back(sh(xi));
    }

    // Создание сегментов Эрмита
    std::vector<HermiteSplineSegment> segments;
    for (size_t i = 0; i < x.size() - 1; ++i) {
        HermiteSplineSegment segment = { x[i], x[i + 1], y[i], y[i + 1], dydx[i], dydx[i + 1] };
        segments.push_back(segment);
    }

    // Точки для интерполяции
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
