// ряды фурье №5

#include <iostream>
#include <vector>
#include <cmath>
#include <functional>


// const double M_PI = 3.1415926535;

// Определение численного интегрирования методом прямоугольников
double numericalIntegration(const std::function<double(double)>& func, double a, double b, int n) {
    double h = (b - a) / n;
    double result = 0.0;
    for (int i = 0; i < n; ++i) {
        double x = a + i * h;
        result += func(x) * h;
    }
    return result;
}

// Вычисление коэффициентов ряда Фурье
void fourierSeries(const std::function<double(double)>& func, double period, int harmonics) {
    double a0, an, bn;
    double omega = 2 * M_PI / period;

    // Вычисление a0
    a0 = (1.0 / period) * numericalIntegration(func, 0, period, 1000);
    std::cout << "a0 = " << a0 << std::endl;

    for (int n = 1; n <= harmonics; ++n) {
        // an
        auto cosComponent = [&](double x) {
            return func(x) * std::cos(n * omega * x);
            };
        an = (2.0 / period) * numericalIntegration(cosComponent, 0, period, 1000);

        // bn
        auto sinComponent = [&](double x) {
            return func(x) * std::sin(n * omega * x);
            };
        bn = (2.0 / period) * numericalIntegration(sinComponent, 0, period, 1000);

        std::cout << "a" << n << " = " << an << ", b" << n << " = " << bn << std::endl;
    }
}

int main() {
    setlocale(LC_ALL, "ru");

    auto func = [](double x) {
        return x;
        };

    double period = 2 * M_PI; // Период функции
    int harmonics = 5; // Количество гармоник для разложения

    std::cout << "Ряд Фурье для функции:" << std::endl;
    fourierSeries(func, period, harmonics);

    return 0;
}
