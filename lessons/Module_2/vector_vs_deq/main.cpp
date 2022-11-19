#include "log_duration.h"
#include <deque>
#include <vector>
#include <iostream>
#include <random>

const long unsigned length_of_range = 50'000'000;

template <typename T>
void MakeRandomVector(long unsigned n, std::vector<T>& v) {
    std::random_device rd;
    std::mt19937 gen(rd());
    for (long unsigned i = 0; i <= n; ++i) {
        std::uniform_int_distribution<> dist(0, n);
        v.push_back(T(dist(gen)));
    }
}

template <typename T>
void MakeRandomDeque(long unsigned n, std::deque<T>& d) {
    std::random_device rd;
    std::mt19937 gen(rd());
    for (long unsigned i = 0; i <= n; ++i) {
        std::uniform_int_distribution<> dist(0, n);
        d.push_back(T(dist(gen)));
    }
}

int main() {
    std::vector<long unsigned> v;
    std::deque<long unsigned> d;

    {
        LOG_DURATION("VECTOR FILLING");
        MakeRandomVector(length_of_range, v);
    }

    {
        LOG_DURATION("DEQUE FILLING");
        MakeRandomDeque(length_of_range, d);
    }
    std::cout << std::endl;
    {
        LOG_DURATION("VECTOR SORTING");
        std::sort(v.begin(), v.end());
    }

    {
        LOG_DURATION("DEQUE SORTING");
        std::sort(d.begin(), d.end());
    }

    return 0;
}