# Импорт необходимых библиотек
import numpy as np      # Для работы с массивами и математическими операциями
import random           # Для генерации случайных чисел и перемешивания
import matplotlib.pyplot as plt  # Для визуализации результатов
from math import sqrt   # Для вычисления квадратного корня (расстояния между городами)

# Функция генерации случайных городов на карте
def generate_cities(n_cities, map_size=100):
    """
    Генерирует массив случайных координат городов.
    Аргументы:
        n_cities - количество городов.
        map_size - размер карты (координаты от 0 до map_size).
    Возвращает:
        numpy-массив координат размером n_cities x 2.
    """
    return np.random.rand(n_cities, 2) * map_size

# Функция расчёта матрицы расстояний между городами
def distance_matrix(cities):
    """
    Вычисляет симметричную матрицу расстояний между всеми городами.
    """
    n = len(cities)
    dist = np.zeros((n, n))
    # Вычисляем расстояния для верхнего треугольника матрицы
    for i in range(n):
        for j in range(i + 1, n):
            dx = cities[i][0] - cities[j][0]
            dy = cities[i][1] - cities[j][1]
            distance = sqrt(dx * dx + dy * dy)
            dist[i][j] = distance
            dist[j][i] = distance
    return dist

# Класс генетического алгоритма для решения задачи коммивояжёра (TSP)
class GeneticAlgorithmTSP:
    def __init__(self, cities, pop_size=50, elite_size=10, mutation_rate=0.01, generations=200):
        """
        Инициализация параметров алгоритма:
           cities - массив координат городов,
           pop_size - размер популяции,
           elite_size - число элитных особей, сохраняемых в поколении,
           mutation_rate - вероятность мутации,
           generations - количество поколений для эволюции.
        """
        self.cities = cities
        self.dist_matrix = distance_matrix(cities)
        self.pop_size = pop_size
        self.elite_size = elite_size
        self.mutation_rate = mutation_rate
        self.generations = generations

    # Создание начальной популяции (набора маршрутов)
    def initial_population(self):
        population = []
        for _ in range(self.pop_size):
            individual = list(range(len(self.cities)))  # Список индексов городов
            random.shuffle(individual)                    # Перемешивание для генерации случайного маршрута
            population.append(individual)
        return population

    # Расчёт приспособленности (fitness) особи как обратной длины маршрута
    def fitness(self, individual):
        total_dist = 0
        for i in range(len(individual)):
            # Суммируем расстояния между текущим и предыдущим городом (замыкание маршрута)
            total_dist += self.dist_matrix[individual[i - 1]][individual[i]]
        return 1 / total_dist  # Чем меньше дистанция, тем выше фитнес

    # Селекция двух родителей методом "рулетки"
    def selection(self, population):
        fitnesses = [self.fitness(ind) for ind in population]
        total_fitness = sum(fitnesses)
        probs = [f / total_fitness for f in fitnesses]
        # Выбираем двух родителей с распределением вероятностей пропорционально их фитнесу
        parents = random.choices(population, weights=probs, k=2)
        return parents

    # Упорядоченный кроссовер (OX) для создания потомка
    def crossover(self, parent1, parent2):
        size = len(parent1)
        start, end = sorted(random.sample(range(size), 2))  # Выбираем случайный сегмент
        child = [None] * size
        # Копируем сегмент из первого родителя
        child[start:end] = parent1[start:end]
        ptr = end
        # Заполняем оставшиеся позиции городами из второго родителя, сохраняя их порядок
        for city in parent2:
            if city not in child:
                if ptr >= size:
                    ptr = 0
                child[ptr] = city
                ptr += 1
        return child

    # Мутация: обмен двух случайно выбранных городов
    def mutate(self, individual):
        if random.random() < self.mutation_rate:
            i, j = random.sample(range(len(individual)), 2)
            individual[i], individual[j] = individual[j], individual[i]
        return individual

    # Эволюция популяции для формирования следующего поколения
    def next_generation(self, current_population):
        # Сортировка популяции по убыванию фитнес-функции
        population_sorted = sorted(current_population, key=self.fitness, reverse=True)
        # Сохраняем элитных особей без изменений
        new_population = population_sorted[:self.elite_size]
        # Заполняем оставшуюся часть популяции потомками, полученными через кроссовер и мутацию
        for _ in range(self.pop_size - self.elite_size):
            parent1, parent2 = self.selection(population_sorted)
            child = self.crossover(parent1, parent2)
            child = self.mutate(child)
            new_population.append(child)
        return new_population

    # Запуск алгоритма и эволюция популяции в течение заданного числа поколений
    def run(self):
        population = self.initial_population()
        progress = []  # Список для отслеживания длины лучшего маршрута в каждом поколении

        best_distance = float('inf')
        best_route = None

        for gen in range(self.generations):
            population = self.next_generation(population)
            # Находим лучшую особь текущей популяции
            current_best = max(population, key=self.fitness)
            current_distance = 1 / self.fitness(current_best)
            progress.append(current_distance)

            if current_distance < best_distance:
                best_distance = current_distance
                best_route = current_best

            # Вывод результатов для текущего поколения
            print(f"Поколение {gen+1}/{self.generations}: Длина маршрута = {current_distance:.2f}")

        return best_route, best_distance, progress

# Функция визуализации динамики улучшения маршрута
def plot_progress(progress):
    plt.figure(figsize=(8, 4))
    plt.plot(progress, marker='o', linestyle='-', color='b')
    plt.title("Прогресс оптимизации маршрута")
    plt.xlabel("Поколение")
    plt.ylabel("Длина маршрута")
    plt.grid(True)
    plt.show()

# Функция отображения найденного маршрута на карте
def plot_route(cities, route):
    plt.figure(figsize=(8, 8))
    # Для замыкания маршрута добавляем начальный город в конец
    route_extended = route + [route[0]]
    route_cities = cities[np.array(route_extended)]
    plt.plot(route_cities[:, 0], route_cities[:, 1], 'o-', color='r')
    plt.title("Оптимальный маршрут")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.grid(True)
    plt.show()

# Основной блок для взаимодействия с пользователем и запуска генетического алгоритма
if __name__ == "__main__":
    print("Генетический алгоритм для решения задачи коммивояжера (TSP)")

    # Ввод параметров пользователем с обработкой исключений (используются значения по умолчанию)
    try:
        n_cities = int(input("Введите количество городов (например, 20): "))
    except Exception as e:
        n_cities = 20
        print("Некорректный ввод. Используем значение по умолчанию: 20")

    try:
        pop_size = int(input("Введите размер популяции (например, 50): "))
    except Exception as e:
        pop_size = 50
        print("Некорректный ввод. Используем значение по умолчанию: 50")

    try:
        elite_size = int(input("Введите количество элитных особей (например, 10): "))
    except Exception as e:
        elite_size = 10
        print("Некорректный ввод. Используем значение по умолчанию: 10")

    try:
        mutation_rate = float(input("Введите вероятность мутации (например, 0.01): "))
    except Exception as e:
        mutation_rate = 0.01
        print("Некорректный ввод. Используем значение по умолчанию: 0.01")

    try:
        generations = int(input("Введите количество поколений (например, 200): "))
    except Exception as e:
        generations = 200
        print("Некорректный ввод. Используем значение по умолчанию: 200")

    # Генерация случайных городов
    cities = generate_cities(n_cities)

    # Инициализация генетического алгоритма с введёнными параметрами
    ga_tsp = GeneticAlgorithmTSP(cities, pop_size, elite_size, mutation_rate, generations)
    best_route, best_distance, progress = ga_tsp.run()

    print("\nЛучший найденный маршрут:")
    print(best_route)
    print(f"Длина маршрута: {best_distance:.2f}")

    # Визуализация результатов: график прогресса и оптимальный маршрут
    plot_progress(progress)
    plot_route(cities, best_route)
