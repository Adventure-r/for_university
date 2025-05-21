#include <iostream>
#include <vector>
#include <limits>
using namespace std;

// Функция для проверки, удовлетворяют ли значения ограничениям
bool satisfiesConstraints(int x1, int x2, double x3) {
    if (2 * x1 - x2 + x3 > 1) return false;
    if (-4 * x1 + 2 * x2 - x3 > 2) return false;
    if (3 * x1 + x3 > 5) return false;
    return true;
}

// Функция для решения задачи с перебором x3 по заданному шагу
void solveIntegerOptimization(double stepSize) {
    const int minVal = -10;  // Минимальное значение для x1 и x2
    const int maxVal = 10;   // Максимальное значение для x1 и x2
    const double minX3 = -10.0, maxX3 = 10.0;  // Диапазон для x3

    double bestF = numeric_limits<double>::max();  // Минимальное значение F
    int bestX1 = 0, bestX2 = 0;
    double bestX3 = 0;

    // Перебираем x1 и x2 как целые числа
    for (int x1 = minVal; x1 <= maxVal; ++x1) {
        for (int x2 = minVal; x2 <= maxVal; ++x2) {
            for (double x3 = minX3; x3 <= maxX3; x3 += stepSize) {
                if (satisfiesConstraints(x1, x2, x3)) {
                    double currentF = x1 - x2 - 3 * x3;

                    // Вывод промежуточных результатов
                    cout << "Intermediate result: x1 = " << x1 << ", x2 = " << x2
                        << ", x3 = " << x3 << ", F = " << currentF << endl;

                    if (currentF < bestF) {
                        bestF = currentF;
                        bestX1 = x1;
                        bestX2 = x2;
                        bestX3 = x3;
                    }
                }
            }
        }
    }

    cout << "\nOptimal solution:" << endl;
    cout << "x1 = " << bestX1 << ", x2 = " << bestX2 << ", x3 = " << bestX3 << endl;
    cout << "Minimal value of the main func: F = " << bestF << endl;
}

int main() {
    setlocale(LC_ALL, "ru");

    double stepSize;
    cout << "Choose step x3 (example, 0.1, 0.01, 0.001): ";
    cin >> stepSize;

    solveIntegerOptimization(stepSize);
    return 0;
}
