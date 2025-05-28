// бЫстрое преобразование Фурье с построением спектра (GNUPlot)
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdio>  // Для работы с popen()

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Определение типа для комплексных чисел
using Complex = std::complex<double>;
using ComplexVector = std::vector<Complex>;

// Функция быстрого преобразования Фурье (FFT)
void FFT(ComplexVector& a) {
    size_t n = a.size();
    if (n == 1)
        return;

    // Разделяем массив на четные и нечетные элементы
    ComplexVector even(n / 2), odd(n / 2);
    for (size_t i = 0; i < n / 2; ++i) {
        even[i] = a[i * 2];
        odd[i] = a[i * 2 + 1];
    }

    // Рекурсивное выполнение FFT для четных и нечетных частей
    FFT(even);
    FFT(odd);

    // Объединяем результаты
    for (size_t k = 0; k < n / 2; ++k) {
        Complex t = std::polar(1.0, -2 * M_PI * k / n) * odd[k];
        a[k] = even[k] + t;
        a[k + n / 2] = even[k] - t;
    }
}

int main() {
    setlocale(LC_ALL, "ru");

    // Исходные данные
    ComplexVector data = { 1, 2, 3, 4, 2, 0, 1, -1 };

    std::cout << "Исходные данные:" << std::endl;
    for (const auto& val : data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Выполнение FFT
    FFT(data);

    std::cout << "Результат преобразования Фурье:" << std::endl;
    for (const auto& val : data) {
        std::cout << val << std::endl;
    }

    // Подготовка данных для построения графика:
    // Вычисляем модуль (амплитуду) каждого комплексного числа FFT
    size_t N = data.size();

    // Запускаем GNUPlot через popen и передаем в него данные для графика
    FILE* gp = popen("gnuplot -persistent", "w");
    if (gp) {
        // Настройка заголовка, подписей осей и сетки
        fprintf(gp, "set title 'Амплитудный спектр FFT'\n");
        fprintf(gp, "set xlabel 'Индекс частоты'\n");
        fprintf(gp, "set ylabel 'Амплитуда'\n");
        fprintf(gp, "set grid\n");
        // Построение графика модулей комплексных чисел
        fprintf(gp, "plot '-' with linespoints title 'Модуль'\n");
        for (size_t i = 0; i < N; i++) {
            double mag = std::abs(data[i]);
            fprintf(gp, "%zu %f\n", i, mag);
        }
        fprintf(gp, "e\n");
        fflush(gp);
        pclose(gp);
    } else {
        std::cerr << "Не удалось открыть GNUPlot. Убедитесь, что GNUPlot установлен и доступен в PATH." << std::endl;
    }

    return 0;
}

