#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <windows.h>  // Для SetConsoleOutputCP
#include <clocale>    // Для setlocale

using namespace std;

const int N = 6; // Размер системы
const double EPS = 1e-6; // Точность
const int MAX_ITER = 100; // Максимальное число итераций

// Пример системы: x_i^2 - 1 = 0, i = 1..6
vector<double> evaluate_system(const vector<double>& x) {
    vector<double> f(N);
    for (int i = 0; i < N; ++i) {
        f[i] = x[i] * x[i] - 1.0;
    }
    return f;
}

// Численное вычисление матрицы Якоби
vector<vector<double>> compute_jacobian(const vector<double>& x, double h = 1e-5) {
    vector<vector<double>> J(N, vector<double>(N));
    for (int i = 0; i < N; ++i) {
        vector<double> x_plus = x;
        x_plus[i] += h;
        vector<double> f_plus = evaluate_system(x_plus);
        vector<double> x_minus = x;
        x_minus[i] -= h;
        vector<double> f_minus = evaluate_system(x_minus);

        for (int j = 0; j < N; ++j) {
            J[j][i] = (f_plus[j] - f_minus[j]) / (2 * h); // Центральная разность
        }
    }
    return J;
}

// Решение системы линейных уравнений методом Гаусса
bool solve_gauss(const vector<vector<double>>& A, const vector<double>& b, vector<double>& x) {
    vector<vector<double>> M = A;
    vector<double> y = b;
    int n = A.size();

    // Прямой ход
    for (int i = 0; i < n; ++i) {
        // Поиск ведущего элемента
        int max_row = i;
        for (int k = i + 1; k < n; ++k) {
            if (abs(M[k][i]) > abs(M[max_row][i])) max_row = k;
        }

        if (abs(M[max_row][i]) < 1e-10) return false; // Матрица вырожденная

        swap(M[i], M[max_row]);
        swap(y[i], y[max_row]);

        double pivot = M[i][i];
        for (int j = i; j < n; ++j) M[i][j] /= pivot;
        y[i] /= pivot;

        for (int k = 0; k < n; ++k) {
            if (k != i && abs(M[k][i]) > 1e-10) {
                double factor = M[k][i];
                for (int j = i; j < n; ++j) M[k][j] -= factor * M[i][j];
                y[k] -= factor * y[i];
            }
        }
    }

    x = y;
    return true;
}

// Метод Ньютона-Рафсона
void newton_raphson(vector<double>& x0) {
    int iter = 0;
    while (iter < MAX_ITER) {
        vector<double> F = evaluate_system(x0);
        vector<vector<double>> J = compute_jacobian(x0);

        // Проверка на достижение точности
        double norm_F = 0.0;
        for (double fi : F) norm_F += fi * fi;
        if (sqrt(norm_F) < EPS) break;

        // Решение J * dx = -F
        vector<double> dx(N);
        if (!solve_gauss(J, F, dx)) {
            cout << "Ошибка: сингулярная матрица Якоби." << endl;
            return;
        }

        // Обновление x
        for (int i = 0; i < N; ++i) {
            x0[i] -= dx[i];
        }

        iter++;
    }

    cout << "Метод Ньютона-Рафсона:\n";
    for (int i = 0; i < N; ++i) {
        cout << "x[" << i << "] ≈ " << fixed << setprecision(6) << x0[i] << endl;
    }
    cout << "Итераций: " << iter << endl;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    // Активация локали для корректного отображения кириллицы
    setlocale(LC_ALL, "ru_RU.UTF-8");
    vector<double> x0 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5}; // Начальное приближение
    newton_raphson(x0);
    return 0;
}