// Быстрое преобразование фурье
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Определение типа для комплексных чисел
using Complex = std::complex<double>;
using ComplexVector = std::vector<Complex>;

void FFT(ComplexVector& a) {
    size_t n = a.size();

    if (n == 1) return;

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

    ComplexVector data = { 1, 2, 3, 4, 2, 0, 1, -1 };

    // Преобразование входного массива в комплексные числа
    std::cout << "Исходные данные:" << std::endl;
    for (const auto& val : data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    FFT(data);

    std::cout << "Результат преобразования Фурье:" << std::endl;
    for (const auto& val : data) {
        std::cout << val << std::endl;
    }

    return 0;
}
