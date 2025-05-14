#include <iostream>
#include <cmath>
#include <windows.h>  // Для SetConsoleOutputCP
#include <clocale>    // Для setlocale


double f(double x) {
    return std::exp(x) + std::exp(-3 * x) - 4;
}

double df(double x) {
    return std::exp(x) - 3 * std::exp(-3 * x);
}

void dichotomy(double a, double b, double eps) {
    double fa = f(a);
    double fb = f(b);
    if (fa * fb >= 0) {
        std::cout << "На интервале [" << a << ", " << b << "] нет корня или их несколько." << std::endl;
        return;
    }
    int iter = 0;
    while (b - a > eps) {
        double c = (a + b) / 2;
        double fc = f(c);
        if (fc == 0 || b - a < eps) break;
        if (fa * fc < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }
        iter++;
    }
    std::cout << "Метод дихотомии: корень ≈ " << (a + b) / 2 << ", итераций: " << iter << std::endl;
}

void newton(double x0, double eps) {
    int iter = 0;
    double x = x0;
    double fx = f(x);
    double dfx = df(x);
    while (std::abs(fx) > eps && iter < 1000) {
        double dx = fx / dfx;
        x -= dx;
        fx = f(x);
        dfx = df(x);
        iter++;
    }
    std::cout << "Метод Ньютона: корень ≈ " << x << ", итераций: " << iter << std::endl;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    // Активация локали для корректного отображения кириллицы
    setlocale(LC_ALL, "ru_RU.UTF-8");
    double a = 1.0;
    double b = 2.0;
    double eps = 1e-6;
    dichotomy(a, b, eps);
    newton(1.5, eps);
    return 0;
}