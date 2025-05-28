# Импорт необходимых библиотек
import numpy as np              # Для работы с массивами и математическими операциями
import random                   # Для генерации случайных чисел, перемешивания и отбора
import matplotlib.pyplot as plt # Для построения и отображения графиков
from math import sqrt           # Для расчёта евклидова расстояния между городами

# ---------------------------------------------------------------------------------
# Функция generate_cities
# Описание:
#    Генерирует случайное распределение городов на карте.
#
# Аргументы:
#    n_cities  - количество городов.
#    map_size  - размер карты (координаты будут от 0 до map_size).
#
# Возвращает:
#    numpy-массив размером (n_cities x 2), где каждая строка содержит координаты города.
# ---------------------------------------------------------------------------------
def generate_cities(n_cities, map_size=100):
    return np.random.rand(n_cities, 2) * map_size

# ---------------------------------------------------------------------------------
# Функция distance_matrix
# Описание:
#    Вычисляет симметричную матрицу расстояний между всеми городами.
#
# Аргументы:
#    cities - массив координат городов.
#
# Возвращает:
#    Квадратную матрицу размером (n_cities x n_cities), где элемент (i, j) — 
#    расстояние между городами i и j.
# ---------------------------------------------------------------------------------
def distance_matrix(cities):
    n = len(cities)
    dist = np.zeros((n, n))
    # Рассчитываем расстояние только для верхнего треугольника, затем зеркально копируем
    for i in range(n):
        for j in range(i + 1, n):
            dx = cities[i][0] - cities[j][0]
            dy = cities[i][1] - cities[j][1]
            distance = sqrt(dx * dx + dy * dy)
            dist[i][j] = distance
            dist[j][i] = distance
    return dist

# ---------------------------------------------------------------------------------
# Класс GeneticAlgorithmTSP
# Описание:
#    Класс реализует генетический алгоритм для решения задачи коммивояжёра.
#    В процессе работы алгоритма создаётся популяция маршрутов (перестановок городов),
#    затем с помощью операторов селекции, кроссовера и мутации осуществляется эволюция.
# ---------------------------------------------------------------------------------
class GeneticAlgorithmTSP:
    # Конструктор класса. Задаются города и параметры алгоритма.
    def __init__(self, cities, pop_size=50, elite_size=10, mutation_rate=0.01, generations=200):
        """
        cities       - массив координат городов,
        pop_size     - размер популяции (количество маршрутов в каждом поколении),
        elite_size   - число элитных особей (лучшие маршруты будут автоматически сохраняться в следующем поколении),
        mutation_rate- вероятность мутации (обмена городов),
        generations  - количество поколений (итераций эволюции).
        """
        self.cities = cities
        self.dist_matrix = distance_matrix(cities)
        self.pop_size = pop_size
        self.elite_size = elite_size
        self.mutation_rate = mutation_rate
        self.generations = generations

    # ---------------------------------------------------------------------------------
    # Метод initial_population
    # Описание:
    #   Создаёт начальную популяцию, состоящую из случайных маршрутов.
    # ---------------------------------------------------------------------------------
    def initial_population(self):
        population = []
        for _ in range(self.pop_size):
            # Каждый маршрут – это перестановка индексов городов
            individual = list(range(len(self.cities)))
            random.shuffle(individual)  # Перемешиваем для получения случайного маршрута
            population.append(individual)
        return population

    # ---------------------------------------------------------------------------------
    # Метод fitness
    # Описание:
    #   Рассчитывает «приспособленность» (fitness) маршрута как обратную величину от 
    #   полной длины маршрута (считается как циклический маршрут – последний город соединён с первым).
    #   Чем короче маршрут, тем выше его fitness.
    # ---------------------------------------------------------------------------------
    def fitness(self, individual):
        total_dist = 0
        # Проходим по всем городам маршрута; individual[i-1] в Python означает последний элемент, 
        # когда i = 0, что позволяет замкнуть маршрут.
        for i in range(len(individual)):
            total_dist += self.dist_matrix[individual[i - 1]][individual[i]]
        return 1 / total_dist  # Чем меньше total_dist, тем больше fitness

    # ---------------------------------------------------------------------------------
    # Метод selection
    # Описание:
    #   Реализует рулеточный отбор: родительский маршрут выбирается случайно, 
    #   но с вероятностью, пропорциональной его fitness.
    # ---------------------------------------------------------------------------------
    def selection(self, population):
        fitnesses = [self.fitness(ind) for ind in population]
        total_fitness = sum(fitnesses)
        probs = [f / total_fitness for f in fitnesses]
        # Выбираем двух родителей с использованием рассчитанных вероятностей
        parents = random.choices(population, weights=probs, k=2)
        return parents

    # ---------------------------------------------------------------------------------
    # Метод crossover
    # Описание:
    #   Упорядоченный кроссовер (OX): выбирается случайный отрезок родителя 1, 
    #   затем оставшиеся города заполняются в порядке их появления во втором родителе, 
    #   сохраняя порядок и расклад.
    # ---------------------------------------------------------------------------------
    def crossover(self, parent1, parent2):
        size = len(parent1)
        start, end = sorted(random.sample(range(size), 2))  # Выбираем случайный сегмент
        child = [None] * size
        # Копируем отрезок родителя 1 в ребёнка
        child[start:end] = parent1[start:end]
        ptr = end
        # Заполняем оставшиеся места городами из родителя 2, если их там ещё нет
        for city in parent2:
            if city not in child:
                if ptr >= size:
                    ptr = 0  # Если достигли конца, начинаем с начала
                child[ptr] = city
                ptr += 1
        return child

    # ---------------------------------------------------------------------------------
    # Метод mutate
    # Описание:
    #   Реализует мутацию маршрута: с вероятностью mutation_rate происходит обмен
    #   двух случайно выбранных городов, что помогает поддерживать генетическое разнообразие.
    # ---------------------------------------------------------------------------------
    def mutate(self, individual):
        if random.random() < self.mutation_rate:
            i, j = random.sample(range(len(individual)), 2)
            individual[i], individual[j] = individual[j], individual[i]
        return individual

    # ---------------------------------------------------------------------------------
    # Метод next_generation
    # Описание:
    #   Создаёт новое поколение, начиная с элитных особей (самых лучших маршрутов).
    #   Затем оставшиеся места заполняются потомками, полученными через процесс селекции,
    #   кроссовера и мутации.
    # ---------------------------------------------------------------------------------
    def next_generation(self, current_population):
        # Сортируем популяцию по fitness (от лучшего к худшему)
        population_sorted = sorted(current_population, key=self.fitness, reverse=True)
        # Сохраняем элиту: лучшие elite_size особей остаются неизменными
        new_population = population_sorted[:self.elite_size]
        # Заполняем оставшуюся часть популяции потомками
        for _ in range(self.pop_size - self.elite_size):
            parent1, parent2 = self.selection(population_sorted)
            child = self.crossover(parent1, parent2)
            child = self.mutate(child)
            new_population.append(child)
        return new_population

    # ---------------------------------------------------------------------------------
    # Метод run
    # Описание:
    #   Запускает генетический алгоритм на протяжении заданного числа поколений.
    #   В каждом поколении вычисляется лучший маршрут и его длина.
    #   В процессе ведётся запись динамики улучшения маршрута.
    # ---------------------------------------------------------------------------------
    def run(self):
        population = self.initial_population()
        progress = []  # Список для отслеживания длины лучшего маршрута в каждом поколении
        best_distance = float('inf')
        best_route = None

        for gen in range(self.generations):
            population = self.next_generation(population)
            # Определяем лучшего кандидата (маршрут с максимальной fitness)
            current_best = max(population, key=self.fitness)
            current_distance = 1 / self.fitness(current_best)
            progress.append(current_distance)
            # Если текущий лучший маршрут лучше предыдущего, обновляем итоговый маршрут
            if current_distance < best_distance:
                best_distance = current_distance
                best_route = current_best
            print(f"Поколение {gen+1}/{self.generations}: Длина маршрута = {current_distance:.2f}")
        return best_route, best_distance, progress

# ---------------------------------------------------------------------------------
# Функция plot_progress
# Описание:
#    Строит график, показывающий, как улучшалась длина лучшего маршрута
#    от поколения к поколению.
# ---------------------------------------------------------------------------------
def plot_progress(progress):
    plt.figure(figsize=(8, 4))
    plt.plot(progress, marker='o', linestyle='-', color='b')
    plt.title("Прогресс оптимизации маршрута")
    plt.xlabel("Поколение")
    plt.ylabel("Длина маршрута")
    plt.grid(True)
    plt.show()

# ---------------------------------------------------------------------------------
# Функция plot_route
# Описание:
#    Строит 2D-карту с оптимальным маршрутом.
#    Маршрут закрывается (последний город соединяется с первым).
# ---------------------------------------------------------------------------------
def plot_route(cities, route):
    plt.figure(figsize=(8, 8))
    # Замыкаем маршрут, добавляя первый город в конец
    route_extended = route + [route[0]]
    # Извлекаем координаты городов согласно порядку в маршруте
    route_cities = cities[np.array(route_extended)]
    plt.plot(route_cities[:, 0], route_cities[:, 1], 'o-', color='r')
    plt.title("Оптимальный маршрут")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.grid(True)
    plt.show()

# ---------------------------------------------------------------------------------
# Основной блок программы для запуска генетического алгоритма TSP
# ---------------------------------------------------------------------------------
if __name__ == "__main__":
    print("Генетический алгоритм для решения задачи коммивояжёра (TSP)")

    # Ввод параметров пользователем (если ввод некорректный — используются значения по умолчанию)
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

    # Генерация случайных городов на карте
    cities = generate_cities(n_cities)

    # Инициализация генетического алгоритма с указанными параметрами
    ga_tsp = GeneticAlgorithmTSP(cities, pop_size, elite_size, mutation_rate, generations)
    
    # Запуск алгоритма: получение лучшего маршрута, его расстояния и динамики улучшения
    best_route, best_distance, progress = ga_tsp.run()

    # Вывод оптимального маршрута и его длины
    print("\nЛучший найденный маршрут:")
    print(best_route)
    print(f"Длина маршрута: {best_distance:.2f}")

    # Визуализация результатов:
    # 1. График прогресса (улучшения длины маршрута от поколения к поколению)
    plot_progress(progress)
    # 2. Отображение оптимального маршрута на карте
    plot_route(cities, best_route)
