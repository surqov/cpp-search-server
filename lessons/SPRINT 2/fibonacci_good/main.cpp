#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

int64_t F(int i) {
    if (i == 0) {
        return 0;
    }

    int64_t prev0 = 0, prev1 = 1;

    for (int t = 1; t < i; ++t) {
        int64_t next = prev0 + prev1;
        prev0 = prev1;
        prev1 = next;
    }

    return prev1;
}

int main() {
    int i;

    while (true) {
        cout << "Введите индекс: ";
        if (!(cin >> i)) {
            break;
        }

        cout << "Fi = " << F(i) << endl;
    }
}