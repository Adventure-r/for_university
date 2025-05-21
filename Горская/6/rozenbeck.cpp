/*
   Пример минимизации нелинейного функционала (функции Розенброка) методом градиентного спуска.
   Функция Розенброка в двумерном пространстве:
       f(x, y) = (1 - x)^2 + 100*(y - x^2)^2
   Минимум достигается при (x, y) = (1, 1), где f(1,1)=0.
*/

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Функция Розенброка
double rosenbrock(const vector<double>& x) {
    // Предполагаем, что вектор x имеет размер 2: x[0] = x, x[1] = y
    return pow(1 - x[0], 2) + 100 * pow(x[1] - x[0]*x[0], 2);
}

// Аналитический градиент функции Розенброка
vector<double> grad_rosenbrock(const vector<double>& x) {
    vector<double> grad(2);
    // df/dx = -2*(1 - x) - 400*x*(y - x^2)
    grad[0] = -2*(1 - x[0]) - 400*x[0]*(x[1] - x[0]*x[0]);
    // df/dy = 200*(y - x^2)
    grad[1] = 200*(x[1] - x[0]*x[0]);
    return grad;
}

int main() {
    // Начальное приближение (часто для функции Розенброка берут (-1.2, 1.0))
    vector<double> x = {-1.2, 1.0};
    
    // Параметры градиентного спуска
    double alpha = 0.001;        // шаг спуска (learning rate)
    double tolerance = 1e-8;     // порог сходимости (если норма градиента меньше tolerance, считаем, что минимум найден)
    int max_iter = 1000000;      // максимальное число итераций

    int iter = 0;
    while (iter < max_iter) {
        vector<double> grad = grad_rosenbrock(x);
        double grad_norm = sqrt(grad[0]*grad[0] + grad[1]*grad[1]);
        
        // Если норма градиента достаточно мала, останавливаем алгоритм
        if (grad_norm < tolerance)
            break;
        
        // Обновление переменных: x = x - alpha * grad
        x[0] -= alpha * grad[0];
        x[1] -= alpha * grad[1];
        
        iter++;

        /*
        if (iter % 100000 == 0) {
            cout << "Итерация " << iter << ": f(x,y) = " << rosenbrock(x) << endl;
        }
    }
    
    cout << "Минимум достигнут за " << iter << " итераций." << endl;
    cout << "Точка минимума: x = " << x[0] << ", y = " << x[1] << endl;
    cout << "Значение функционала в точке минимума: f(x,y) = " << rosenbrock(x) << endl;
  
    */
        // Вывод промежуточных результатов каждые 100000 итераций (опционально)
        if (iter % 100000 == 0) {
            cout << "Iteration " << iter << ": f(x,y) = " << rosenbrock(x) << endl;
        }
    }
    
    cout << "Minimum reached in " << iter << " iterations." << endl;
    cout << "Minimum point: x = " << x[0] << ", y = " << x[1] << endl;
    cout << "Function value at the minimum: f(x,y) = " << rosenbrock(x) << endl;
    
    return 0;
}
