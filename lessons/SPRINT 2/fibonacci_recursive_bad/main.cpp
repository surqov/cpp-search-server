#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

// числа Фибоначчи довольно быстро растут, используем int64_t
int64_t F(int i) {
    // тут обработаем i == 0 и i == 1
    if (i <= 0) {
        return 0;
    }

    if (i == 1) {
        return 1;
    }

    // рекурсивно вызовем саму функцию F
    return F(i - 1) + F(i - 2);
}

int main() {
    int i;

    while (true) {
        cout << "Введите индекс: "s;
        if (!(cin >> i)) {
            break;
        }

        cout << "Fi = "s << F(i) << endl;
    }
} 