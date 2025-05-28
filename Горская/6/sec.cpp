#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>    // Для записи данных в файл
#include <cstdio>     // Для вызова gnuplot

using namespace std;

// Функция Розенброка: f(x, y) = (1 - x)^2 + 100*(y - x^2)^2
double rosenbrock(const vector<double>& x) {
    // Предполагаем, что x[0] = x, x[1] = y.
    return pow(1 - x[0], 2) + 100 * pow(x[1] - x[0] * x[0], 2);
}

// Аналитический градиент функции Розенброка.
vector<double> grad_rosenbrock(const vector<double>& x) {
    vector<double> grad(2);
    grad[0] = -2 * (1 - x[0]) - 400 * x[0] * (x[1] - x[0] * x[0]);
    grad[1] = 200 * (x[1] - x[0] * x[0]);
    return grad;
}

// Гессиан функции Розенброка.
// Возвращает матрицу 2x2, представленную в виде вектора векторов.
vector<vector<double>> hessian_rosenbrock(const vector<double>& x) {
    vector<vector<double>> H(2, vector<double>(2, 0.0));
    H[0][0] = 2 - 400 * x[1] + 1200 * x[0] * x[0]; // d²f/dx²
    H[0][1] = -400 * x[0];                         // d²f/dxdy
    H[1][0] = -400 * x[0];                         // d²f/dydx
    H[1][1] = 200;                                 // d²f/dy²
    return H;
}

// Функция для вычисления евклидовой нормы вектора (2D случай).
double norm(const vector<double>& v) {
    return sqrt(v[0] * v[0] + v[1] * v[1]);
}

int main() {
    // Начальное приближение (типичный выбор для функции Розенброка: (-1.2, 1.0))
    vector<double> x = {-1.2, 1.0};

    // Параметры метода Ньютона.
    double tolerance = 1e-8; // Если норма градиента меньше tolerance, считаем, что минимум достигнут.
    int max_iter = 100;      // Метод Ньютона обычно сходится быстро.
    int iter = 0;

    // Открываем файл для записи траектории (сохраним все итерации, т.к. их мало).
    ofstream outfile("rosenbrock_newton_path.dat");
    // Заголовок файла (опционально):
    outfile << "# iter x y f(x,y)" << endl;
    // Записываем начальную точку.
    outfile << iter << " " << x[0] << " " << x[1] << " " << rosenbrock(x) << endl;

    // Итерационный процесс метода Ньютона.
    while (iter < max_iter) {
        vector<double> grad = grad_rosenbrock(x);
        double grad_norm = norm(grad);

        // Если норма градиента достаточно мала, выходим из цикла.
        if (grad_norm < tolerance)
            break;

        vector<vector<double>> H = hessian_rosenbrock(x);

        // Вычисляем детерминант гессиана.
        double det = H[0][0] * H[1][1] - H[0][1] * H[1][0];
        if (fabs(det) < 1e-12) {
            cout << "The Hessian is singular or nearly singular." << endl;
            break;
        }

        // Вычисляем поправочный шаг: delta = H^{-1} * grad.
        // Для 2x2 матрицы: H^{-1} = (1/det)*[[H[1][1], -H[0][1]], [-H[1][0], H[0][0]]].
        vector<double> delta(2);
        delta[0] = ( H[1][1] * grad[0] - H[0][1] * grad[1] ) / det;
        delta[1] = (-H[1][0] * grad[0] + H[0][0] * grad[1] ) / det;

        // Обновление переменных: x = x - delta.
        x[0] -= delta[0];
        x[1] -= delta[1];

        iter++;

        // Записываем текущую итерацию в файл.
        outfile << iter << " " << x[0] << " " << x[1] << " " << rosenbrock(x) << endl;

        // Выводим промежуточную информацию.
        cout << "Iteration " << iter
             << ": f(x,y) = " << rosenbrock(x)
             << ", gradient norm = " << grad_norm << endl;
    }
    outfile.close();

    cout << "\nMinimum reached in " << iter << " iterations." << endl;
    cout << "Minimum point: x = " << x[0] << ", y = " << x[1] << endl;
    cout << "Function value at the minimum: f(x,y) = " << rosenbrock(x) << endl;

    // Рисование графика с помощью gnuplot.
    // Здесь мы будем выводить контурную карту функции Розенброка и траекторию, сохранённую в файле.
    FILE* gp = popen("gnuplot -persistent", "w");
    if (gp) {
        // Настройка оформления графика.
	fprintf(gp, "set term qt enhanced");
	fprintf(gp, "set mouse");
        fprintf(gp, "set title 'Траектория метода Ньютона для функции Розенброка'\n");
        fprintf(gp, "set xlabel 'x'\n");
        fprintf(gp, "set ylabel 'y'\n");
        fprintf(gp, "set grid\n");
        
        // Если хотите сгладить контуры, можно установить dgrid3d.
        fprintf(gp, "set dgrid3d 30,30,2\n");
        
        // Определяем функцию Розенброка в синтаксисе gnuplot. Обратите внимание на '**' для степени.
        fprintf(gp, "f(x,y) = (1 - x)**2 + 100*(y - x**2)**2\n");

        // Рисуем контурную карту и накладываем траекторию.
        // Здесь splot строит 3D-график, но с 'set view map' мы получаем проекцию 2D.
        fprintf(gp, "set view map\n");
        fprintf(gp, "set contour base\n");
        fprintf(gp, "unset surface\n");
        fprintf(gp, "set cntrparam levels 15\n");
        fprintf(gp, "splot f(x,y) with lines lc rgb 'blue' notitle, \\\n");
        // Используем все три столбца из файла: столбец 2 (x), столбец 3 (y) и столбец 4 (f)
        fprintf(gp, "      'rosenbrock_newton_path.dat' using 2:3:4 with linespoints pt 7 lc rgb 'red' title 'Траектория'\n");
        fprintf(gp, "pause -1 'Press any key to exit'\n");

        fflush(gp);
        pclose(gp);
    } else {
        cerr << "Не удалось открыть gnuplot. Убедитесь, что gnuplot установлен." << endl;
    }

    return 0;
}
