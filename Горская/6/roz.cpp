#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>    // Для сохранения данных пути в файл
#include <cstdio>     // Для вызова gnuplot через popen

using namespace std;

// Функция Розенброка: f(x,y) = (1 - x)^2 + 100*(y - x^2)^2
double rosenbrock(const vector<double>& x) {
    // Предполагаем, что вектор x имеет размер 2: x[0] = x, x[1] = y.
    return pow(1 - x[0], 2) + 100 * pow(x[1] - x[0]*x[0], 2);
}

// Аналитический градиент функции Розенброка.
vector<double> grad_rosenbrock(const vector<double>& x) {
    vector<double> grad(2);
    // Производная по x: df/dx = -2*(1 - x) - 400*x*(y - x^2)
    grad[0] = -2*(1 - x[0]) - 400*x[0]*(x[1] - x[0]*x[0]);
    // Производная по y: df/dy = 200*(y - x^2)
    grad[1] = 200*(x[1] - x[0]*x[0]);
    return grad;
}

int main() {
    // Начальное приближение для метода (типичный выбор для функции Розенброка)
    vector<double> x = {-1.2, 1.0};

    // Параметры градиентного спуска
    double alpha = 0.001;       // Шаг спуска (learning rate)
    double tolerance = 1e-8;      // Порог сходимости по норме градиента
    int max_iter = 1000000;       // Максимальное число итераций
    int iter = 0;
    int record_step = 1000;       // Интервал записи текущей точки в файл (чтобы не сохранять все 10⁶ итераций)

    // Открываем файл для записи траектории спуска
    ofstream outfile("rosenbrock_path.dat");
    // Записываем заголовок (опционально, для удобства)
    outfile << "# iter x y f(x,y)\n";

    // Основной цикл градиентного спуска
    while (iter < max_iter) {
        vector<double> grad = grad_rosenbrock(x);
        double grad_norm = sqrt(grad[0]*grad[0] + grad[1]*grad[1]);

        // Если норма градиента достаточно мала, считаем, что минимум достигнут
        if (grad_norm < tolerance)
            break;

        // Обновление значений: x = x - alpha*grad(x)
        x[0] -= alpha * grad[0];
        x[1] -= alpha * grad[1];

        iter++;

        // Сохраняем данные каждые "record_step" итераций
        if (iter % record_step == 0) {
            double f_val = rosenbrock(x);
            outfile << iter << " " << x[0] << " " << x[1] << " " << f_val << "\n";
            // Выводим промежуточную информацию каждые 100000 итераций
            if (iter % 100000 == 0) {
                cout << "Iteration " << iter << ": f(x,y) = " << f_val << endl;
            }
        }
    }
    outfile.close();

    cout << "Minimum reached in " << iter << " iterations." << endl;
    cout << "Minimum point: x = " << x[0] << ", y = " << x[1] << endl;
    cout << "Function value at the minimum: f(x,y) = " << rosenbrock(x) << endl;

    // Рисование графика с помощью gnuplot.
    // График содержит:
    // - Фоновый контур функции Розенброка,
    // - Траекторию (путь) спуска, сохранённую в файле "rosenbrock_path.dat".
    FILE* gp = popen("gnuplot -persistent", "w");
    if (gp) {
        // Заголовок, подписи и сетка
        fprintf(gp, "set title 'Путь градиентного спуска по функции Розенброка'\n");
        fprintf(gp, "set xlabel 'x'\n");
        fprintf(gp, "set ylabel 'y'\n");
        fprintf(gp, "set grid\n");

        // Определяем функцию Розенброка в gnuplot (синтаксис gnuplot: возведение в степень — **)
        fprintf(gp, "f(x,y) = (1 - x)**2 + 100*(y - x**2)**2\n");

        // Рисуем контурную карту:
        fprintf(gp, "set contour base\n");
        fprintf(gp, "unset surface\n");
        fprintf(gp, "set view map\n");
        fprintf(gp, "set cntrparam levels 10\n");

        // Команда splot рисует контур карты функции и накладывает траекторию спуска.
        // В файле "rosenbrock_path.dat" столбцы: 1 - итерация, 2 - x, 3 - y, 4 - f(x,y).
        // Здесь мы используем столбцы 2 и 3 для построения траектории.
        fprintf(gp, "splot f(x,y) with lines lc rgb 'blue' notitle, \
      'rosenbrock_path.dat' using 2:3:4 with linespoints pt 7 lc rgb 'red' title 'Траектория'");
        fprintf(gp, "      'rosenbrock_path.dat' using 2:3 with linespoints pt 7 lc rgb 'red' title 'Траектория'\n");
        fprintf(gp, "pause -1 'Нажмите любую клавишу для выхода'\n");
        fflush(gp);
        pclose(gp);
    } else {
        cerr << "Не удалось открыть gnuplot. Убедитесь, что gnuplot установлен." << endl;
    }

    return 0;
}
