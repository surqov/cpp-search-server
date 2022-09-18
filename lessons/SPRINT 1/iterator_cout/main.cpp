#include <iostream>
#include <string>
#include <algorithm>
#include <set>
#include <vector>

using namespace std;

template <typename IterType>
void PrintRange(IterType begin, IterType end) {
    for (auto i = begin; i != end; ++i) {
        cout << *i << " ";
    }
    cout << endl;
}

int main() {
    cout << "Test1"s << endl;
    set<int> test1 = {1, 1, 1, 2, 3, 4, 5, 5};
    PrintRange(test1.begin(), test1.end());
    cout << "Test2"s << endl;
    vector<int> test2 = {}; // пустой контейнер
    PrintRange(test2.begin(), test2.end());
    cout << "End of tests"s << endl;
    return 1;
}