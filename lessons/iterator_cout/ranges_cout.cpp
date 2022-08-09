#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Container, typename Element>
void FindAndPrint(const Container& container, const Element& element_to_find) {
    auto it_begin = begin(container);
    auto it_end = end(container);
    auto it = find(it_begin, it_end, element_to_find);
    PrintRange(it_begin, it);
    PrintRange(it, it_end);
}

int main() {
    set<int> test = {1, 1, 1, 2, 3, 4, 5, 5};
    FindAndPrint(test, 3);
    FindAndPrint(test, 0);
    return 0;
}
