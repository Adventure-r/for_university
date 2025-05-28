/*
   Пример минимизации функции Розенброка методом Ньютона (второго порядка).

   Функция Розенброка в 2D:
       f(x, y) = (1 - x)^2 + 100*(y - x^2)^2

   Минимум достигается в точке (x, y) = (1, 1), где f(1,1)=0.
*/

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Функция Розенброка.
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

// Функция для вычисления евклидовой нормы вектора (для 2-мерного случая)
double norm(const vector<double>& v) {
    return sqrt(v[0] * v[0] + v[1] * v[1]);
}

int main() {
    // Начальное приближение (часто для функции Розенброка используют (-1.2, 1.0))
    vector<double> x = {-1.2, 1.0};
    
    // Параметры метода Ньютона.
    double tolerance = 1e-8; // Критерий сходимости, если норма градиента меньше tolerance
    int max_iter = 100;      // Максимальное число итераций (метод второго порядка быстро сходится)
    int iter = 0;
    
    while (iter < max_iter) {
        vector<double> grad = grad_rosenbrock(x);
        double grad_norm = norm(grad);
        
        // Если норма градиента достаточно мала, считаем, что минимум достигнут.
        if (grad_norm < tolerance)
            break;
        
        vector<vector<double>> H = hessian_rosenbrock(x);
        
        // Вычисляем определитель Гессиана.
        double det = H[0][0] * H[1][1] - H[0][1] * H[1][0];
        if (fabs(det) < 1e-12) {
            // cout << "Гессиан вырожден или близок к вырождению." << endl;
            cout << "The Hessian is singular or nearly singular." << endl;
            break;
        }
        
        // Вычисляем обратный Гессиан для 2x2 матрицы:
        // Для матрицы [[A, B], [B, C]] обратная матрица: (1/det)*[[C, -B], [-B, A]]
        vector<double> delta(2);
        delta[0] = ( H[1][1] * grad[0] - H[0][1] * grad[1] ) / det;
        delta[1] = (-H[1][0] * grad[0] + H[0][0] * grad[1] ) / det;
        
        // Обновление переменных по методу Ньютона: x = x - H^{-1} * grad
        x[0] -= delta[0];
        x[1] -= delta[1];
        
       /* 
    cout << "Итерация " << iter 
            << ": f(x, y) = " << rosenbrock(x)
            << ", норма градиента = " << grad_norm << endl;
    }
    
    cout << "\nМинимум достигнут за " << iter << " итераций." << endl;
    cout << "Точка минимума: x = " << x[0] << ", y = " << x[1] << endl;
    cout << "Значение функции в точке минимума: f(x, y) = " << rosenbrock(x) << endl;
    */
   iter++;
    cout << "Iteration " << iter 
        << ": f(x,y) = " << rosenbrock(x)
        << ", gradient norm = " << grad_norm << endl;
    }
    
    cout << "\nMinimum reached in " << iter << " iterations." << endl;
    cout << "Minimum point: x = " << x[0] << ", y = " << x[1] << endl;
    cout << "Function value at the minimum: f(x,y) = " << rosenbrock(x) << endl;
   
    return 0;
}
