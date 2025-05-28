#include <iostream>
#include <vector>
#include <clocale> 
using namespace std;

// Функция для решения задачи о рюкзаке с динамическим программированием
int maximizeLoot(int maxCapacity, const vector<int>& itemWeights, const vector<int>& itemValues, int itemCount, vector<int>& selectedItems, int& totalValue) {
    vector<vector<int>> lootTable(itemCount + 1, vector<int>(maxCapacity + 1, 0));

    // Заполняем таблицу динамического программирования
    for (int i = 1; i <= itemCount; ++i) {
        for (int capacity = 0; capacity <= maxCapacity; ++capacity) {
            if (itemWeights[i - 1] <= capacity) {
                lootTable[i][capacity] = max(
                    lootTable[i - 1][capacity],
                    lootTable[i - 1][capacity - itemWeights[i - 1]] + itemValues[i - 1]
                );
            }
            else {
                lootTable[i][capacity] = lootTable[i - 1][capacity];
            }
        }
    }

    // Восстановление списка выбранных предметов
    int remainingCapacity = maxCapacity;
    totalValue = 0;
    for (int i = itemCount; i > 0; --i) {
        if (lootTable[i][remainingCapacity] != lootTable[i - 1][remainingCapacity]) {
            selectedItems.push_back(i - 1); // Добавляем индекс предмета
            remainingCapacity -= itemWeights[i - 1];
            totalValue += itemValues[i - 1]; // Считаем сумму ценностей выбранных предметов
        }
    }

    return lootTable[itemCount][maxCapacity]; // Возвращаем максимальную возможную ценность
}

int main() {
    setlocale(LC_ALL, "ru");

    int itemCount, maxCapacity;
    cout << "Count of items: ";
    cin >> itemCount;
    cout << "Max backpack's capacity : ";
    cin >> maxCapacity;

    vector<int> itemWeights(itemCount), itemValues(itemCount);
    cout << "Weight of items: ";
    for (int i = 0; i < itemCount; ++i) {
        cin >> itemWeights[i];
    }
    cout << "Value of items: ";
    for (int i = 0; i < itemCount; ++i) {
        cin >> itemValues[i];
    }

    vector<int> selectedItems;
    int totalValue = 0;
    int bestValue = maximizeLoot(maxCapacity, itemWeights, itemValues, itemCount, selectedItems, totalValue);

    // Вывод результатов
    cout << "Possible maxed value: " << bestValue << endl;
    cout << "Sum of values of choosen items: " << totalValue << endl;
    cout << "Count of choosen items: " << selectedItems.size() << endl;
    cout << "Choosen items:" << endl;
    for (int index : selectedItems) {
        cout << "- Item " << index + 1 << " (Weight: " << itemWeights[index] << ", Value: " << itemValues[index] << ")" << endl;
    }

    return 0;
}
