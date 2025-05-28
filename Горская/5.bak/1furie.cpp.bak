#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <cstdio>

// Если константа M_PI не определена, можно её задать вручную
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Метод численного интегрирования (метод прямоугольников)
double numericalIntegration(const std::function<double(double)>& func, double a, double b, int n) {
    double h = (b - a) / n;
    double result = 0.0;
    for (int i = 0; i < n; ++i) {
        double x = a + i * h;
        result += func(x) * h;
    }
    return result;
}

int main() {
    setlocale(LC_ALL, "ru");

    // Исходная функция: f(x) = x
    auto func = [](double x) {
        return x;
    };

    double period = 2 * M_PI;         // Период функции
    int harmonics = 5;                // Количество гармоник для разложения
    double omega = 2 * M_PI / period;   // Угловая частота
    int integrationSteps = 1000;      // Число разбиений для интегрирования

    // Векторы для хранения коэффициентов ряда Фурье.
    // coeffA[0] соответствует a0, а coeffA[n] и coeffB[n] — коэффициентам для гармоники с номером n.
    std::vector<double> coeffA(harmonics + 1, 0.0);
    std::vector<double> coeffB(harmonics + 1, 0.0); // coeffB[0] не используется

    // Вычисление a0
    coeffA[0] = (1.0 / period) * numericalIntegration(func, 0, period, integrationSteps);
    std::cout << "a0 = " << coeffA[0] << std::endl;

    // Вычисление коэффициентов a_n и b_n для n >= 1
    for (int n = 1; n <= harmonics; ++n) {
        auto cosComponent = [&, n](double x) {
            return func(x) * std::cos(n * omega * x);
        };
        auto sinComponent = [&, n](double x) {
            return func(x) * std::sin(n * omega * x);
        };

        coeffA[n] = (2.0 / period) * numericalIntegration(cosComponent, 0, period, integrationSteps);
        coeffB[n] = (2.0 / period) * numericalIntegration(sinComponent, 0, period, integrationSteps);

        std::cout << "a" << n << " = " << coeffA[n] << ", b" << n << " = " << coeffB[n] << std::endl;
    }

    // Подготовка данных для построения графиков
    int plotPoints = 500;               // Число точек для графиков
    std::vector<double> x_vals(plotPoints + 1);
    std::vector<double> y_orig(plotPoints + 1);
    std::vector<double> y_fourier(plotPoints + 1);
    double dx = period / plotPoints;

    for (int i = 0; i <= plotPoints; i++) {
        double x = i * dx;
        x_vals[i] = x;
        // Значение исходной функции
        y_orig[i] = func(x);
        // Вычисление приближения функции рядом Фурье:
        double approxim = coeffA[0]; // Первый член ряда
        for (int n = 1; n <= harmonics; n++) {
            approxim += coeffA[n] * std::cos(n * omega * x) + coeffB[n] * std::sin(n * omega * x);
        }
        y_fourier[i] = approxim;
    }

    // Передача данных в gnuplot для построения графиков
    // Проверяем, получен ли дескриптор потока
    FILE* gp = popen("gnuplot -persistent", "w");
    if (gp) {
        // Настройка заголовка, подписей осей и сетки
        fprintf(gp, "set title 'Функция и её приближение рядом Фурье'\n");
        fprintf(gp, "set xlabel 'x'\n");
        fprintf(gp, "set ylabel 'y'\n");
        fprintf(gp, "set grid\n");
        // Построение двух графиков: сначала исходной функции, затем приближения
        fprintf(gp, "plot '-' with lines title 'Оригинальная функция', '-' with lines title 'Приближение ряда Фурье'\n");

        // Передача данных для исходной функции
        for (int i = 0; i <= plotPoints; i++) {
            fprintf(gp, "%f %f\n", x_vals[i], y_orig[i]);
        }
        fprintf(gp, "e\n");

        // Передача данных для ряда Фурье
        for (int i = 0; i <= plotPoints; i++) {
            fprintf(gp, "%f %f\n", x_vals[i], y_fourier[i]);
        }
        fprintf(gp, "e\n");
        fflush(gp);
        pclose(gp);
    } else {
        std::cerr << "Не удалось открыть gnuplot. Убедитесь, что gnuplot установлен." << std::endl;
    }

    return 0;
}

