// Кубический сплайн

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

/*
 *	Кубический сплайн лучше обычного так как учитывается и вторая производная функции, для подбора значений
 *
 * */

// Структура для коэффициентов сплайнов
struct Spline {
    double a, b, c, d, x;
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
    return std::exp(-x);
}

// Построение кубического сплайна (системы уравнений для коэф куб сплайна)
std::vector<Spline> buildCubicSpline(const std::vector<double>& x, const std::vector<double>& y) {
    int n = x.size() - 1;
    std::vector<double> h(n), alpha(n), l(n + 1), mu(n + 1), z(n + 1);
    std::vector<double> c(n + 1), b(n), d(n);

    // Шаги сетки h[i] = 0.04
    for (int i = 0; i < n; ++i) {
        h[i] = x[i + 1] - x[i];
    }

    // Вычисление alpha - вторые производные (выражает изменение угла наклона между соседними интервалами)
    for (int i = 1; i < n; ++i) {
        alpha[i] = (3.0 / h[i]) * (y[i + 1] - y[i]) - (3.0 / h[i - 1]) * (y[i] - y[i - 1]);
    }

    // Прогонка (для нахождения коэф системы трехдиагональных уравнений)
    l[0] = 1.0; // диагональные эл-ты матрицы
    mu[0] = 0.0; // вспомогательные значения для упрощения расчетов
    z[0] = 0.0; // правые части уравнений для 2-й произвоодной
    for (int i = 1; i < n; ++i) {
        l[i] = 2.0 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    l[n] = 1.0;
    z[n] = 0.0;
    c[n] = 0.0;

    // Обратная прогонка
    for (int j = n - 1; j >= 0; --j) {
        c[j] = z[j] - mu[j] * c[j + 1]; // коэф 2-й производной
        b[j] = (y[j + 1] - y[j]) / h[j] - h[j] * (c[j + 1] + 2.0 * c[j]) / 3.0; // коэф 1-й производной
        d[j] = (c[j + 1] - c[j]) / (3.0 * h[j]); // коэф 3-й производной
    }

    // Формирование коэффициентов сплайна
    std::vector<Spline> splines(n);
    for (int i = 0; i < n; ++i) {
        splines[i] = { y[i], b[i], c[i], d[i], x[i] };
    }
    return splines;
}

// Интерполяция с использованием сплайна
double interpolate(const std::vector<Spline>& splines, double x_value) {
    // Найти нужный интервал
    const Spline* spline = nullptr;
    for (const auto& s : splines) {
        if (x_value >= s.x && x_value <= s.x + 0.04) { // Шаг = 0.04
            spline = &s;
            break;
        }
    }

    if (!spline) {
        throw std::out_of_range("Значение x выходит за границы интерполяции.");
    }

    // Вычислить значение сплайна
    double dx = x_value - spline->x;
    return spline->a + spline->b * dx + spline->c * dx * dx + spline->d * dx * dx * dx;
}

int main() {
    setlocale(LC_ALL, "ru");
    // Данные
    std::vector<double> x = {1.00, 1.04, 1.08, 1.12, 1.16, 1.2} ;
    std::vector<double> y;
    for (double xi : x) {
        y.push_back(ch(xi)); // Вычисляем значения ch(x)
    }

    // Построение сплайна
    std::vector<Spline> splines = buildCubicSpline(x, y);

    // Точки для интерполяции
    std::vector<double> points = {  1.05, 1.09, 1.13, 1.15, 1.17, 1.20 };
    std::cout << std::fixed << std::setprecision(6);

    std::cout << "Интерполированные значения:\n";
    for (double p : points) {
        try {
            double result = interpolate(splines, p);
            std::cout << "S(" << p << ") = " << result << "\n";
        }
        catch (const std::out_of_range& e) {
            std::cerr << e.what() << "\n";
        }
    }

    return 0;
}
