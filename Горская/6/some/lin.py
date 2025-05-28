import numpy as np
from scipy.optimize import linprog

# Данные задачи
products = ['Π₁', 'Π₂', 'Π₃', 'Π₄']
income_coeff = [2, 3, 4, 5]  # Доход от единицы продукции

# Матрица затрат ресурсов (площадь, труд, тяга)
resource_coeff = [
    [2, 3, 4, 5],  # Площадь
    [2, 1, 2, 4],  # Труд
    [2, 3, 1, 1]   # Тяга
]

resources_available = [100, 120, 80]  # Доступные ресурсы
resource_names = ['Площадь', 'Труд', 'Тяга']

# Решение задачи линейного программирования
# Минимизируем отрицательный доход (эквивалентно максимизации)
result = linprog(
    c=[-x for x in income_coeff],  # Отрицательные коэффициенты для максимизации
    A_ub=resource_coeff,           # Коэффициенты ограничений
    b_ub=resources_available,      # Доступные ресурсы
    bounds=(0, None),              # Переменные >= 0
    method='highs'
)

# Проверка успешности решения
if not result.success:
    raise ValueError("Решение не найдено")

# Извлекаем результаты
production_plan = result.x
max_income = -result.fun  # Возвращаем знак (минимизировали отрицательный доход)

# Вычисляем использование ресурсов
used_resources = np.dot(resource_coeff, production_plan)

# Вывод результатов
print("Оптимальный план производства:")
for product, amount in zip(products, production_plan):
    print(f"{product}: {amount:.1f} единиц")

print(f"\nМаксимальный доход: {max_income:.2f} денежных единиц")

print("\nИспользование ресурсов:")
for name, used, avail in zip(resource_names, used_resources, resources_available):
    percentage = used / avail * 100
    print(f"{name}: {used:.1f} из {avail} ({percentage:.1f}%)")

# Анализ дефицитных ресурсов
print("\nАнализ ограничений:")
for i, (name, used, avail) in enumerate(zip(resource_names, used_resources, resources_available)):
    if abs(used - avail) < 1e-6:  # Учитываем погрешность вычислений
        print(f"{name} является лимитирующим ресурсом (использован полностью)")
    else:
        print(f"{name} имеет резерв: {avail - used:.1f} единиц")
