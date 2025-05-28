#include <iostream>
#include <vector>
#include <stdexcept> // Библиотека для обработки исключений

/*
 *     Ввод исходных данных:

        Заданы массивы значений x и y, описывающие функцию.

        Например, x = {1, 2, 3, 4, 5} и y = {3, 3.5, 3.67, 3.75, 3.8} представляют функцию.

    Линейная интерполяция:

        Допустим, мы ищем y при x = 2.5.

        Находим интервал: x = [2, 3], y = [3.5, 3.67].

        Используем формулу.
	
	проверяем, не выходит ли за границы результат
*/

// Функция для линейной интерполяции
double linearInterpolation(const std::vector<double>& x, const std::vector<double>& y, double x_value) {
    if (x.size() != y.size()) { // Проверяем, что массивы x и y имеют одинаковую длину
        throw std::invalid_argument("Размеры массивов x и y должны совпадать.");
    }

    // Проходим по всем интервалам между соседними точками
    for (size_t i = 0; i < x.size() - 1; ++i) {
        if (x_value >= x[i] && x_value <= x[i + 1]) { // Проверяем, попадает ли x_value в интервал
            // Линейная интерполяция по формуле прямой
            return y[i] + (y[i + 1] - y[i]) * (x_value - x[i]) / (x[i + 1] - x[i]);
        }
    }

    // Если x_value выходит за границы массива, выбрасываем исключение
    throw std::out_of_range("Значение x находится вне диапазона данных.");
}

int main() {
    setlocale(LC_ALL, "ru");

    // Исходные точки (x, y)
    std::vector<double> x = { 1, 2, 3, 4, 5 };      // x-координаты
    std::vector<double> y = { 3, 3.5, 3.67, 3.75, 3.8 }; // y-координаты

    double x_value = 4; // Точка, для которой ищем интерполированное значение

    try {
        double interpolated_value = linearInterpolation(x, y, x_value); // Вызываем функцию
        std::cout << "Интерполированное значение y для x = " << x_value << " равно " << interpolated_value << std::endl;
    }
    catch (const std::exception& e) { // Обрабатываем возможные ошибки
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}

