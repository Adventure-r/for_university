#include <iostream>
#include <cmath>
#include <vector>
#include <windows.h>  // Для SetConsoleOutputCP
#include <clocale>    // Для setlocale

using namespace std;

struct System {
    double operator()(double x, double y) const {
        return cos(y) + x - 1.5;
    }
};

struct System2 {
    double operator()(double x, double y) const {
        return 2 * y - sin(x - 0.5) - 1;
    }
};

struct Jacobian {
    vector<vector<double>> operator()(double x, double y) const {
        vector<vector<double>> J(2, vector<double>(2));
        J[0][0] = 1.0;
        J[0][1] = -sin(y);
        J[1][0] = -cos(x - 0.5);
        J[1][1] = 2.0;
        return J;
    }
};

vector<double> solve_linear(const vector<vector<double>>& A, const vector<double>& b) {
    double det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
    if (det == 0) throw runtime_error("Singular matrix");
    double x = (b[0] * A[1][1] - b[1] * A[0][1]) / det;
    double y = (A[0][0] * b[1] - A[1][0] * b[0]) / det;
    return {x, y};
}

void newton_system(double x0, double y0, double eps) {
    System F;
    System2 G;
    Jacobian J;
    int iter = 0;
    double x = x0;
    double y = y0;
    vector<double> delta(2);
    while (true) {
        double f_val = F(x, y);
        double g_val = G(x, y);
        if (abs(f_val) < eps && abs(g_val) < eps) break;
        auto jac = J(x, y);
        vector<double> b = {-f_val, -g_val};
        try {
            delta = solve_linear(jac, b);
        } catch (...) {
            cout << "Ошибка: сингулярная матрица" << endl;
            return;
        }
        x += delta[0];
        y += delta[1];
        iter++;
        if (iter > 1000) {
            cout << "Не сходится" << endl;
            return;
        }
    }
    cout << "Метод Ньютона-Рафсона: x ≈ " << x << ", y ≈ " << y << ", итераций: " << iter << endl;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    // Активация локали для корректного отображения кириллицы
    setlocale(LC_ALL, "ru_RU.UTF-8");
    double x0 = 0.5;
    double y0 = 0.5;
    double eps = 1e-6;
    newton_system(x0, y0, eps);
    return 0;
}