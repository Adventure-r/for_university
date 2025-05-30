import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.animation as animation

# ============================================================================
# ТЕОРЕТИЧЕСКАЯ ОСНОВА
# ============================================================================
"""
🔬 ТЕОРЕТИЧЕСКИЕ ОСНОВЫ
1. Уравнение Лапласа:

Формула: ∇²u = ∂²u/∂x² + ∂²u/∂y² = 0
Физический смысл: Описывает стационарные процессы без источников (температура, потенциал, мембрана)
Тип: Эллиптическое уравнение в частных производных

2. Метод конечных разностей:

Аппроксимация производных: Заменяем производные разностными отношениями
Формула для внутренних узлов: u[i,j] = (u[i±1,j] + u[i,j±1])/4
Порядок точности: O(h²) для квадратной сетки

3. Метод Гаусса-Зейделя:

Особенность: Использует уже обновленные значения соседних узлов
Преимущество: Быстрее сходится, чем метод Якоби
Критерий остановки: Максимальная невязка < заданной точности


🎯 КЛЮЧЕВЫЕ МОМЕНТЫ ДЛЯ ЭКЗАМЕНА
Алгоритм решения (пошагово):

Дискретизация области → создание сетки узлов
Аппроксимация оператора Лапласа → конечно-разностная схема
Установка граничных условий → фиксация значений на границе
Итерационное решение → метод Гаусса-Зейделя до сходимости
Анализ результата → проверка точности и физической корректности

Критические формулы:

Дискретный лапласиан: (u[i+1,j] + u[i-1,j] + u[i,j+1] + u[i,j-1] - 4u[i,j])/h² = 0
Итерация Гаусса-Зейделя: u[i,j]^(new) = (сумма соседей)/4
Критерий сходимости: max|u^(n+1) - u^(n)| < ε


⚠️ ПОТЕНЦИАЛЬНЫЕ ОШИБКИ И ИСКЛЮЧЕНИЯ
Распространенные ошибки:

Индексация массивов: u[j,i] соответствует точке (x[i], y[j])
Граничные условия: Не забывать фиксировать границы
Сходимость: Может не сходиться для плохо обусловленных задач
Устойчивость: Зависит от отношения шагов сетки

Ограничения метода:

Работает только для областей, покрываемых прямоугольной сеткой
Требует граничных условий Дирихле
Скорость сходимости зависит от размера сетки (O(h²) итераций)
"""
"""
УРАВНЕНИЕ ЛАПЛАСА: ∇²u = ∂²u/∂x² + ∂²u/∂y² = 0

Это эллиптическое уравнение в частных производных, описывающее стационарные
процессы без источников/стоков (установившееся распределение температуры,
электростатический потенциал, деформация мембраны и т.д.)

МЕТОД КОНЕЧНЫХ РАЗНОСТЕЙ:
Заменяем производные конечно-разностными схемами на сетке:

∂²u/∂x² ≈ (u[i+1,j] - 2*u[i,j] + u[i-1,j]) / h²
∂²u/∂y² ≈ (u[i,j+1] - 2*u[i,j] + u[i,j-1]) / k²

где h, k - шаги сетки по x и y соответственно

Для квадратной сетки (h = k): u[i,j] = (u[i+1,j] + u[i-1,j] + u[i,j+1] + u[i,j-1])/4

МЕТОД ГАУССА-ЗЕЙДЕЛЯ:
Итерационный метод решения системы линейных уравнений:
u[i,j]^(n+1) = (u[i+1,j]^(n) + u[i-1,j]^(n+1) + u[i,j+1]^(n) + u[i,j-1]^(n+1))/4
"""

class LaplaceEquationSolver:
    """
    Класс для решения уравнения Лапласа на прямоугольной области
    методом конечных разностей с использованием итерации Гаусса-Зейделя
    """
    
    def __init__(self, Lx=1.0, Ly=1.0, nx=50, ny=50):
        """
        Инициализация параметров задачи
        
        Параметры:
        ----------
        Lx, Ly : float
            Размеры прямоугольной области по x и y
        nx, ny : int
            Количество узлов сетки по x и y (включая границы)
        """
        # Геометрические параметры
        self.Lx = Lx  # Длина области по x
        self.Ly = Ly  # Длина области по y
        self.nx = nx  # Число узлов по x
        self.ny = ny  # Число узлов по y
        
        # Вычисляем шаги сетки
        self.dx = Lx / (nx - 1)  # Шаг по x
        self.dy = Ly / (ny - 1)  # Шаг по y
        
        # Создаем координатные сетки
        self.x = np.linspace(0, Lx, nx)  # Узлы по x
        self.y = np.linspace(0, Ly, ny)  # Узлы по y
        self.X, self.Y = np.meshgrid(self.x, self.y)  # 2D сетки координат
        
        # Инициализируем решение нулями
        self.u = np.zeros((ny, nx))  # u[j,i] соответствует точке (x[i], y[j])
        
        # Массивы для хранения истории итераций (для анимации)
        self.solution_history = []
        self.residual_history = []
        
        print(f"Инициализирована сетка {nx}×{ny}")
        print(f"Шаги сетки: dx = {self.dx:.4f}, dy = {self.dy:.4f}")
    
    def set_boundary_conditions(self, boundary_func=None):
        """
        Установка граничных условий Дирихле: u = f(x,y) на границе
        
        Параметры:
        ----------
        boundary_func : callable or None
            Функция граничных условий f(x, y)
            Если None, используются условия по умолчанию
        """
        if boundary_func is None:
            # Граничные условия по умолчанию:
            # u = 100*sin(π*x) на верхней границе (y = Ly)
            # u = 0 на остальных границах
            
            # Нижняя граница (y = 0)
            self.u[0, :] = 0
            
            # Верхняя граница (y = Ly)
            self.u[-1, :] = 100 * np.sin(np.pi * self.x / self.Lx)
            
            # Левая граница (x = 0)
            self.u[:, 0] = 0
            
            # Правая граница (x = Lx)
            self.u[:, -1] = 0
            
            print("Установлены граничные условия по умолчанию:")
            print("  u = 0 на трех границах")
            print("  u = 100*sin(π*x) на верхней границе")
        else:
            # Пользовательские граничные условия
            # Нижняя граница
            for i in range(self.nx):
                self.u[0, i] = boundary_func(self.x[i], 0)
            
            # Верхняя граница
            for i in range(self.nx):
                self.u[-1, i] = boundary_func(self.x[i], self.Ly)
            
            # Левая граница
            for j in range(self.ny):
                self.u[j, 0] = boundary_func(0, self.y[j])
            
            # Правая граница
            for j in range(self.ny):
                self.u[j, -1] = boundary_func(self.Lx, self.y[j])
            
            print("Установлены пользовательские граничные условия")
    
    def gauss_seidel_iteration(self):
        """
        Выполняет одну итерацию метода Гаусса-Зейделя
        
        АЛГОРИТМ ГАУССА-ЗЕЙДЕЛЯ:
        1. Проходим по всем внутренним узлам сетки
        2. Для каждого узла вычисляем новое значение как среднее от четырех соседей
        3. Используем уже обновленные значения соседних узлов (ключевая особенность!)
        
        Возвращает:
        -----------
        max_residual : float
            Максимальная невязка на данной итерации
        """
        max_residual = 0.0
        
        # Проходим по всем внутренним узлам (исключаем границы)
        for j in range(1, self.ny - 1):  # от 1 до ny-2 включительно
            for i in range(1, self.nx - 1):  # от 1 до nx-2 включительно
                
                # Сохраняем старое значение для вычисления невязки
                u_old = self.u[j, i]
                
                # КЛЮЧЕВАЯ ФОРМУЛА метода Гаусса-Зейделя:
                # Новое значение = среднее арифметическое четырех соседей
                self.u[j, i] = 0.25 * (
                    self.u[j, i+1] +     # правый сосед
                    self.u[j, i-1] +     # левый сосед (уже обновлен!)
                    self.u[j+1, i] +     # верхний сосед
                    self.u[j-1, i]       # нижний сосед (уже обновлен!)
                )
                
                # Вычисляем локальную невязку (изменение за итерацию)
                residual = abs(self.u[j, i] - u_old)
                max_residual = max(max_residual, residual)
        
        return max_residual
    
    def solve(self, tolerance=1e-6, max_iterations=10000, save_history=True):
        """
        Решает уравнение Лапласа итерационным методом Гаусса-Зейделя
        
        Параметры:
        ----------
        tolerance : float
            Критерий сходимости (максимальная невязка)
        max_iterations : int
            Максимальное число итераций
        save_history : bool
            Сохранять ли историю итераций для анимации
        
        Возвращает:
        -----------
        iterations : int
            Количество выполненных итераций
        converged : bool
            Флаг сходимости
        """
        print(f"Начинаем итерационное решение...")
        print(f"Критерий сходимости: {tolerance}")
        print(f"Максимум итераций: {max_iterations}")
        
        # Очищаем историю
        if save_history:
            self.solution_history = []
            self.residual_history = []
        
        for iteration in range(max_iterations):
            # Выполняем одну итерацию Гаусса-Зейделя
            max_residual = self.gauss_seidel_iteration()
            
            # Сохраняем состояние для анимации
            if save_history and (iteration % 10 == 0 or max_residual < tolerance):
                self.solution_history.append(self.u.copy())
                self.residual_history.append(max_residual)
            
            # Выводим прогресс каждые 100 итераций
            if iteration % 100 == 0:
                print(f"Итерация {iteration:5d}: невязка = {max_residual:.2e}")
            
            # Проверяем критерий сходимости
            if max_residual < tolerance:
                print(f"Сходимость достигнута за {iteration + 1} итераций")
                print(f"Финальная невязка: {max_residual:.2e}")
                return iteration + 1, True
        
        print(f"Достигнуто максимальное число итераций ({max_iterations})")
        print(f"Финальная невязка: {max_residual:.2e}")
        return max_iterations, False
    
    def compute_analytical_solution(self):
        """
        Вычисляет аналитическое решение для граничных условий по умолчанию
        
        Для граничных условий:
        u(x,0) = 0, u(x,Ly) = 100*sin(π*x/Lx), u(0,y) = u(Lx,y) = 0
        
        Аналитическое решение методом разделения переменных:
        u(x,y) = 100 * sin(π*x/Lx) * sinh(π*y/Lx) / sinh(π*Ly/Lx)
        
        Возвращает:
        -----------
        u_analytical : ndarray
            Массив аналитического решения
        """
        # Проверяем, используются ли граничные условия по умолчанию
        top_boundary = 100 * np.sin(np.pi * self.x / self.Lx)
        if not np.allclose(self.u[-1, :], top_boundary, rtol=1e-10):
            print("Предупреждение: аналитическое решение доступно только для граничных условий по умолчанию")
            return None
        
        # Вычисляем аналитическое решение
        u_analytical = np.zeros_like(self.u)
        
        for j in range(self.ny):
            for i in range(self.nx):
                x, y = self.x[i], self.y[j]
                u_analytical[j, i] = (100 * np.sin(np.pi * x / self.Lx) * 
                                    np.sinh(np.pi * y / self.Lx) / 
                                    np.sinh(np.pi * self.Ly / self.Lx))
        
        return u_analytical
    
    def plot_solution(self, show_analytical=True, figsize=(15, 5)):
        """
        Визуализирует численное решение и сравнивает с аналитическим
        
        Параметры:
        ----------
        show_analytical : bool
            Показывать ли аналитическое решение
        figsize : tuple
            Размер фигуры
        """
        # Вычисляем аналитическое решение
        u_analytical = self.compute_analytical_solution()
        
        if show_analytical and u_analytical is not None:
            fig, axes = plt.subplots(1, 3, figsize=figsize)
        else:
            fig, axes = plt.subplots(1, 2, figsize=(10, 5))
            axes = [axes[0], axes[1], None]
        
        # Цветовая карта
        cmap = plt.cm.viridis
        
        # 1. Численное решение
        im1 = axes[0].contourf(self.X, self.Y, self.u, levels=50, cmap=cmap)
        axes[0].set_title('Численное решение\n(метод Гаусса-Зейделя)', fontsize=12)
        axes[0].set_xlabel('x')
        axes[0].set_ylabel('y')
        axes[0].set_aspect('equal')
        plt.colorbar(im1, ax=axes[0], label='u(x,y)')
        
        # 2. 3D визуализация
        ax_3d = fig.add_subplot(1, 3 if axes[2] else 2, 2, projection='3d')
        surf = ax_3d.plot_surface(self.X, self.Y, self.u, cmap=cmap, alpha=0.9)
        ax_3d.set_title('3D представление')
        ax_3d.set_xlabel('x')
        ax_3d.set_ylabel('y')
        ax_3d.set_zlabel('u(x,y)')
        
        # 3. Сравнение с аналитическим решением
        if show_analytical and u_analytical is not None and axes[2] is not None:
            error = np.abs(self.u - u_analytical)
            im3 = axes[2].contourf(self.X, self.Y, error, levels=50, cmap='Reds')
            axes[2].set_title(f'Абсолютная погрешность\nmax = {np.max(error):.2e}', fontsize=12)
            axes[2].set_xlabel('x')
            axes[2].set_ylabel('y')
            axes[2].set_aspect('equal')
            plt.colorbar(im3, ax=axes[2], label='|u_численное - u_аналитическое|')
            
            # Выводим статистику ошибки
            print(f"\nСтатистика погрешности:")
            print(f"Максимальная абсолютная ошибка: {np.max(error):.2e}")
            print(f"Средняя абсолютная ошибка: {np.mean(error):.2e}")
            print(f"RMS ошибка: {np.sqrt(np.mean(error**2)):.2e}")
        
        plt.tight_layout()
        plt.show()
    
    def plot_convergence(self):
        """
        Строит график сходимости итерационного процесса
        """
        if not self.residual_history:
            print("История сходимости не сохранена")
            return
        
        plt.figure(figsize=(10, 6))
        plt.semilogy(self.residual_history, 'b-o', markersize=4)
        plt.xlabel('Итерация × 10')
        plt.ylabel('Максимальная невязка')
        plt.title('Сходимость метода Гаусса-Зейделя')
        plt.grid(True, alpha=0.3)
        plt.show()
        
        # Анализ скорости сходимости
        if len(self.residual_history) > 10:
            # Оценка коэффициента сходимости
            ratio = self.residual_history[-1] / self.residual_history[-10]
            convergence_rate = ratio**(1/9)  # за 9 шагов
            print(f"Коэффициент сходимости ≈ {convergence_rate:.3f}")
    
    def animate_solution(self, filename=None, interval=200):
        """
        Создает анимацию процесса итераций
        
        Параметры:
        ----------
        filename : str or None
            Имя файла для сохранения (если None, показывает интерактивно)
        interval : int
            Интервал между кадрами в миллисекундах
        """
        if not self.solution_history:
            print("История решения не сохранена")
            return
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
        
        # Определяем пределы для цветовой шкалы
        vmin = min(np.min(sol) for sol in self.solution_history)
        vmax = max(np.max(sol) for sol in self.solution_history)
        
        def animate(frame):
            ax1.clear()
            ax2.clear()
            
            # Левая панель - решение
            im1 = ax1.contourf(self.X, self.Y, self.solution_history[frame], 
                              levels=50, cmap='viridis', vmin=vmin, vmax=vmax)
            ax1.set_title(f'Итерация {frame * 10}')
            ax1.set_xlabel('x')
            ax1.set_ylabel('y')
            ax1.set_aspect('equal')
            
            # Правая панель - сходимость
            if frame > 0:
                ax2.semilogy(self.residual_history[:frame+1], 'b-o', markersize=4)
            ax2.set_xlabel('Итерация × 10')
            ax2.set_ylabel('Невязка')
            ax2.set_title('Сходимость')
            ax2.grid(True, alpha=0.3)
            
            return [im1]
        
        anim = animation.FuncAnimation(fig, animate, frames=len(self.solution_history),
                                     interval=interval, blit=False, repeat=True)
        
        if filename:
            anim.save(filename, writer='pillow')
            print(f"Анимация сохранена в {filename}")
        else:
            plt.show()
        
        return anim

# ============================================================================
# ДЕМОНСТРАЦИЯ ИСПОЛЬЗОВАНИЯ
# ============================================================================

def main():
    """
    Главная функция - демонстрирует все возможности решателя
    """
    print("="*70)
    print("РЕШЕНИЕ УРАВНЕНИЯ ЛАПЛАСА МЕТОДОМ КОНЕЧНЫХ РАЗНОСТЕЙ")
    print("="*70)
    
    # 1. Создаем экземпляр решателя
    print("\n1. ИНИЦИАЛИЗАЦИЯ")
    solver = LaplaceEquationSolver(Lx=1.0, Ly=1.0, nx=51, ny=51)
    
    # 2. Устанавливаем граничные условия
    print("\n2. ГРАНИЧНЫЕ УСЛОВИЯ")
    solver.set_boundary_conditions()
    
    # 3. Решаем уравнение
    print("\n3. ИТЕРАЦИОННОЕ РЕШЕНИЕ")
    iterations, converged = solver.solve(tolerance=1e-6, max_iterations=5000)
    
    # 4. Визуализация результатов
    print("\n4. ВИЗУАЛИЗАЦИЯ")
    solver.plot_solution()
    
    # 5. График сходимости
    print("\n5. АНАЛИЗ СХОДИМОСТИ")
    solver.plot_convergence()
    
    # 6. Дополнительный анализ
    print("\n6. ДОПОЛНИТЕЛЬНЫЙ АНАЛИЗ")
    
    # Проверяем выполнение уравнения Лапласа во внутренних точках
    laplacian = np.zeros_like(solver.u)
    dx2, dy2 = solver.dx**2, solver.dy**2
    
    for j in range(1, solver.ny-1):
        for i in range(1, solver.nx-1):
            laplacian[j,i] = ((solver.u[j,i+1] - 2*solver.u[j,i] + solver.u[j,i-1])/dx2 +
                             (solver.u[j+1,i] - 2*solver.u[j,i] + solver.u[j-1,i])/dy2)
    
    print(f"Максимальное значение лапласиана: {np.max(np.abs(laplacian)):.2e}")
    print(f"(должно быть близко к нулю для точного решения)")
    
    # Энергия системы (интеграл от градиента)
    grad_x = np.gradient(solver.u, solver.dx, axis=1)
    grad_y = np.gradient(solver.u, solver.dy, axis=0)
    energy = 0.5 * np.sum(grad_x**2 + grad_y**2) * solver.dx * solver.dy
    print(f"Энергия системы: {energy:.6f}")
    
    print("\n" + "="*70)
    print("РЕШЕНИЕ ЗАВЕРШЕНО")
    print("="*70)

# Демонстрация различных граничных условий
def demo_different_boundaries():
    """
    Демонстрирует решение с разными граничными условиями
    """
    print("\n" + "="*50)
    print("ДЕМОНСТРАЦИЯ РАЗЛИЧНЫХ ГРАНИЧНЫХ УСЛОВИЙ")
    print("="*50)
    
    # Пример 1: Четыре разные границы
    def boundary_condition_1(x, y):
        if y == 0:  # нижняя граница
            return 0
        elif y == 1:  # верхняя граница
            return 100 * x * (1 - x)
        elif x == 0:  # левая граница
            return 50 * y
        else:  # правая граница (x == 1)
            return 25 * y
    
    solver1 = LaplaceEquationSolver(nx=41, ny=41)
    solver1.set_boundary_conditions(boundary_condition_1)
    solver1.solve(tolerance=1e-6)
    
    plt.figure(figsize=(12, 5))
    plt.subplot(1, 2, 1)
    plt.contourf(solver1.X, solver1.Y, solver1.u, levels=50, cmap='viridis')
    plt.colorbar(label='u(x,y)')
    plt.title('Граничные условия:\nu(x,0)=0, u(x,1)=100x(1-x)\nu(0,y)=50y, u(1,y)=25y')
    plt.xlabel('x')
    plt.ylabel('y')
    
    # Пример 2: Круговая симметрия на квадрате
    def boundary_condition_2(x, y):
        # Приближение к круговой функции на границе квадрата
        return 100 * np.sin(2 * np.pi * x) * np.sin(2 * np.pi * y)
    
    solver2 = LaplaceEquationSolver(nx=41, ny=41)
    solver2.set_boundary_conditions(boundary_condition_2)
    solver2.solve(tolerance=1e-6)
    
    plt.subplot(1, 2, 2)
    plt.contourf(solver2.X, solver2.Y, solver2.u, levels=50, cmap='plasma')
    plt.colorbar(label='u(x,y)')
    plt.title('Граничные условия:\nu = 100sin(2πx)sin(2πy) на границе')
    plt.xlabel('x')
    plt.ylabel('y')
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    # Основная демонстрация
    main()
    
    # Дополнительные примеры
    demo_different_boundaries()
    
    print("\n🎉 Поздравляю! Теперь вы эксперт по решению уравнения Лапласа!")
    print("Используйте этот код как справочный материал для получения 110/100 баллов! 🚀")