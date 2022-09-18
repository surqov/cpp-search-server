#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt begin, RandomIt end) {
    int dist = distance(begin, end-1);
    if (begin != end) {
        auto mid = begin + dist / 2;
        MergeSort(begin, mid);
        MergeSort(mid, end);
        inplace_merge(begin, mid, end);
    }
}

template <typename RandomIt>
void PrintRange(RandomIt range_begin, RandomIt range_end) {
    for(;range_begin != range_end; ++range_begin) {
        cout << *range_begin << " "s;
    }
    cout << endl;
}

int main() {
    vector<int> test_vector = {3, 7, 5, 6};

    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    // iota(test_vector.begin(), test_vector.end(), 1);

    // random_shuffle   -> https://ru.cppreference.com/w/cpp/algorithm/random_shuffle
    // Перемешивает элементы в случайном порядке
    // shuffle(test_vector.begin(), test_vector.end());

    // Выводим вектор до сортировки
    PrintRange(test_vector.begin(), test_vector.end());

    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());

    // Выводим результат
    PrintRange(test_vector.begin(), test_vector.end());

    return 0;
} 