#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Прототипы функций
vector<double> gaussSeidel(const vector<vector<double>>& A, const vector<double>& b, double tol = 1e-6, int maxIter = 1000);
vector<double> fletcherReeves(const vector<vector<double>>& A, const vector<double>& b, double tol = 1e-6, int maxIter = 1000);
vector<double> tikhonov(const vector<vector<double>>& A, const vector<double>& b, double lambda = 0.01);

void printVector(const vector<double>& v); // Вывод вектора
vector<double> matVecMul(const vector<vector<double>>& A, const vector<double>& x); // Умножение матрицы на вектор
double dot(const vector<double>& a, const vector<double>& b); // Скалярное произведение
vector<vector<double>> transpose(const vector<vector<double>>& A); // Транспонирование матрицы
vector<vector<double>> matMul(const vector<vector<double>>& A, const vector<vector<double>>& B); // Умножение матриц

// Функция для вывода элементов вектора
void printVector(const vector<double>& v) {
    for (double val : v) {
        cout << val << " ";
    }
    cout << endl;
}

// Метод Гаусса-Зейделя — итерационный метод решения СЛАУ Ax = b.
// Использует уже обновлённые значения при вычислении следующих компонент.
vector<double> gaussSeidel(const vector<vector<double>>& A, const vector<double>& b, double tol, int maxIter) {
    int n = b.size(); // Размерность системы
    vector<double> x(n, 0.0); // Начальное приближение — нулевой вектор
    vector<double> x_new = x; // Вектор для новых значений на каждой итерации

    for (int iter = 0; iter < maxIter; iter++) {
        bool converged = true;
        for (int i = 0; i < n; i++) {
            if (fabs(A[i][i]) < 1e-10) {
                cerr << "Zero diagonal element detected at index " << i << endl;
                return x;
            }

            // Вычисляем часть суммы до диагонального элемента (уже обновлённые значения)
            double sum1 = 0.0;
            for (int j = 0; j < i; j++) {
                sum1 += A[i][j] * x_new[j];
            }

            // Вычисляем часть суммы после диагонального элемента (ещё не обновлённые)
            double sum2 = 0.0;
            for (int j = i + 1; j < n; j++) {
                sum2 += A[i][j] * x[j];
            }

            // Обновляем текущую компоненту решения
            x_new[i] = (b[i] - sum1 - sum2) / A[i][i];
        }

        // Проверяем разницу между старым и новым решением
        double error = 0.0;
        for (int i = 0; i < n; i++) {
            error += fabs(x_new[i] - x[i]);
            x[i] = x_new[i]; // Сохраняем новые значения
        }

        if (error < tol) break; // Если ошибка мала — система сошлась
    }

    return x; // Возвращаем найденное решение
}

// Умножение матрицы A на вектор x
vector<double> matVecMul(const vector<vector<double>>& A, const vector<double>& x) {
    int n = A.size();
    vector<double> result(n, 0.0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result[i] += A[i][j] * x[j]; // По определению умножения матрицы на вектор
        }
    }
    return result;
}

// Скалярное произведение двух векторов
double dot(const vector<double>& a, const vector<double>& b) {
    double result = 0.0;
    for (int i = 0; i < a.size(); i++) {
        result += a[i] * b[i];
    }
    return result;
}

// Транспонирование квадратной матрицы
vector<vector<double>> transpose(const vector<vector<double>>& A) {
    int n = A.size();
    vector<vector<double>> AT(n, vector<double>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            AT[i][j] = A[j][i]; // Транспонирование: A^T[i][j] = A[j][i]
        }
    }
    return AT;
}

// Умножение двух матриц
vector<vector<double>> matMul(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int n = A.size();
    vector<vector<double>> result(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += A[i][k] * B[k][j]; // По правилу умножения матриц
            }
        }
    }
    return result;
}

// Метод Флетчера-Ривса — метод сопряжённых градиентов, применённый к нормальным уравнениям:
// Решает систему A^T*A*x = A^T*b, где A может быть прямоугольной или плохо обусловленной.
vector<double> fletcherReeves(const vector<vector<double>>& A, const vector<double>& b, double tol, int maxIter) {
    int n = b.size();

    // Вычисляем A^T и затем A^T*A, A^T*b
    vector<vector<double>> AT = transpose(A);
    vector<vector<double>> ATA = matMul(AT, A); // A^T * A
    vector<double> ATb = matVecMul(AT, b);      // A^T * b

    vector<double> x(n, 0.0); // Начальное приближение
    vector<double> r = ATb;   // Остаток: r = A^T b - A^T A x (при x=0 это просто A^T b)
    vector<double> p = r;     // Направление спуска — начально совпадает с остатком

    for (int iter = 0; iter < maxIter; iter++) {
        vector<double> Ap = matVecMul(ATA, p); // Умножаем матрицу на направление
        double alpha = dot(r, r) / dot(p, Ap); // Шаг по направлению p

        // Обновляем решение и остаток
        for (int i = 0; i < n; i++) {
            x[i] += alpha * p[i];
            r[i] -= alpha * Ap[i];
        }

        // Коэффициент beta для нового направления
        double beta = dot(r, r) / dot(p, p);

        // Обновляем направление по формуле Флетчера-Ривса
        for (int i = 0; i < n; i++) {
            p[i] = r[i] + beta * p[i];
        }

        // Проверяем сходимость
        if (sqrt(dot(r, r)) < tol) break;
    }

    return x; // Возвращаем найденное решение
}

// Метод регуляризации Тихонова — используется для стабилизации решения плохо обусловленных систем.
// Добавляет λI к матрице A^T*A, чтобы избежать вырожденности.
vector<double> tikhonov(const vector<vector<double>>& A, const vector<double>& b, double lambda) {
    int n = b.size();

    // Вычисляем A^T, A^T*A и A^T*b
    vector<vector<double>> AT = transpose(A);
    vector<vector<double>> ATA = matMul(AT, A); // A^T * A
    vector<double> ATb = matVecMul(AT, b);       // A^T * b

    // Добавляем регуляризацию: λ * I
    for (int i = 0; i < n; i++) {
        ATA[i][i] += lambda;
    }

    // Решаем регуляризованную систему методом Гаусса-Зейделя
    return gaussSeidel(ATA, ATb, 1e-6, 1000);
}

// Основная функция программы
int main() {
    // Исходная система линейных уравнений Ax = b
    vector<vector<double>> A = {
        {3.0, 0.0, -1.0},
        {2.0, -5.0, 1.0},
        {20.0, 2.0, 5.0}
    };
    vector<double> b = {7.0, -2.0, 1.0};

    // Решение методом Гаусса-Зейделя
    cout << "Gauss-Seidel solution: ";
    vector<double> x_gs = gaussSeidel(A, b, 1e-6, 1000);
    printVector(x_gs);

    // Решение методом Флетчера-Ривса
    cout << "Fletcher-Reeves solution: ";
    vector<double> x_fr = fletcherReeves(A, b, 1e-6, 1000);
    printVector(x_fr);

    // Решение методом регуляризации Тихонова
    cout << "Tikhonov regularization solution (lambda = 0.01): ";
    vector<double> x_tikh = tikhonov(A, b, 0.01);
    printVector(x_tikh);

    return 0;
}