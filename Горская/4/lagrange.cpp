// полинома Лагранжа

#include <iostream>
#include <vector>
#include <stdexcept>

double linearInterpolation(const std::vector<double>& x, const std::vector<double>& y, double x_value) {
    if (x.size() != y.size()) {
        throw std::invalid_argument("Размеры массивов x и y должны совпадать.");
    }

    for (size_t i = 0; i < x.size() - 1; ++i) {
        if (x_value >= x[i] && x_value <= x[i + 1]) {
            // Линейная интерполяция между двумя точками
            return y[i] + (y[i + 1] - y[i]) * (x_value - x[i]) / (x[i + 1] - x[i]);
        }
    }

    throw std::out_of_range("Значение x находится вне диапазона данных.");
}

int main() {
    setlocale(LC_ALL, "ru");

    std::vector<double> x = { 1, 2, 3, 4, 5 };
    std::vector<double> y = { 3, 3.5, 3.67, 3.75, 3.8 };

    double x_value = 6;

    try {
        double interpolated_value = linearInterpolation(x, y, x_value);
        std::cout << "Интерполированное значение y для x = " << x_value << " равно " << interpolated_value << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
