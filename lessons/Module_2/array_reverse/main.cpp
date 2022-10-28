#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

template <typename T>
void ReverseArray(T* start, size_t size) {
    if (size == 1 || size == 0) {
        return;
    }
    T* end_ = &start[size - 1];
    while (start <= end_) {
        swap(*start, *end_);
        ++start;
        --end_;
    }
}

int main() {
    using namespace std;
    
    vector<int> v = {1, 2};
    ReverseArray(v.data(), v.size());
    assert(v == (vector<int>{2, 1}));
    
    vector<int> g = {1, 3, 10, 12, 150, 151};
    ReverseArray(g.data(), g.size());
    for (auto i : g) {
        cout << i << endl;
    }
}