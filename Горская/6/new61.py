def print_tableau(tableau, basis, iteration):
    """Выводит симплекс-таблицу с базисными переменными и значениями."""
    print(f"\nИтерация {iteration}:")
    headers = ["x1", "x2", "x3", "C", "s1", "s2", "s3", "RHS"]
    print("Базис | " + " | ".join(f"{h:>6}" for h in headers))
    print("-" * 60)
    for i, row in enumerate(tableau[:-1]):
        print(f"{basis[i]:>5} | " + " | ".join(f"{x:>6.2f}" for x in row))
    print("Z     | " + " | ".join(f"{x:>6.2f}" for x in tableau[-1]))
    print("-" * 60)

def simplex_method():
    # Начальная симплекс-таблица
    # Строки: s1, s2, s3, Z
    # Столбцы: x1, x2, x3, C, s1, s2, s3, RHS
    tableau = [
        [-2, -1, 0, 3, 1, 0, 0, 0],  # -2x1 - x2 + 3C + s1 = 0
        [0, -2, -3, 2, 0, 1, 0, 0],   # -2x2 - 3x3 + 2C + s2 = 0
        [1, 1, 1, 0, 0, 0, 1, 500],   # x1 + x2 + x3 + s3 = 500
        [0, 0, 0, -1, 0, 0, 0, 0]      # Z - C = 0 (максимизация C)
    ]
    basis = ["s1", "s2", "s3"]  # Начальные базисные переменные
    iteration = 0

    print("Начальная симплекс-таблица:")
    print_tableau(tableau, basis, iteration)

    while True:
        iteration += 1
        # Шаг 1: Находим ведущий столбец (самый отрицательный коэффициент в строке Z)
        z_row = tableau[-1]
        pivot_col = min(range(len(z_row) - 1), key=lambda j: z_row[j] if z_row[j] < 0 else float('inf'))
        if all(z_row[j] >= 0 for j in range(len(z_row) - 1)):
            print("Найдено оптимальное решение.")
            break

        # Шаг 2: Находим ведущую строку (минимальное положительное отношение RHS/коэффициент)
        ratios = []
        for i in range(len(tableau) - 1):
            coeff = tableau[i][pivot_col]
            rhs = tableau[i][-1]
            if coeff > 0:
                ratios.append((rhs / coeff, i))
            else:
                ratios.append((float('inf'), i))
        pivot_row = min(ratios, key=lambda x: x[0])[1]

        # Шаг 3: Пивотный элемент
        pivot = tableau[pivot_row][pivot_col]
        if pivot == 0:
            print("Ошибка: пивотный элемент равен нулю.")
            break

        # Шаг 4: Делаем пивотный элемент равным 1
        for j in range(len(tableau[pivot_row])):
            tableau[pivot_row][j] /= pivot

        # Обновляем базис
        basis[pivot_row] = ["x1", "x2", "x3", "C", "s1", "s2", "s3"][pivot_col]

        # Шаг 5: Обновляем остальные строки
        for i in range(len(tableau)):
            if i != pivot_row:
                factor = tableau[i][pivot_col]
                for j in range(len(tableau[i])):
                    tableau[i][j] -= factor * tableau[pivot_row][j]

        print_tableau(tableau, basis, iteration)

    # Извлекаем результаты
    result = {"x1": 0, "x2": 0, "x3": 0, "C": 0}
    for i, var in enumerate(basis):
        if var in result:
            result[var] = tableau[i][-1]

    print("\nОптимальный план раскроя:")
    print(f"Количество прутков по первому варианту (x1): {result['x1']}")
    print(f"Количество прутков по второму варианту (x2): {result['x2']}")
    print(f"Количество прутков по третьему варианту (x3): {result['x3']}")
    print(f"Максимальное количество комплектов (C): {result['C']}")

# Запускаем симплекс-метод
simplex_method()