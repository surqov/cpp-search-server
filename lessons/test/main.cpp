#include <iostream>
#include <cmath>

int main() {
    long int a = std::pow(2, 10), b = std::pow(2, 100);
    std::cout << (static_cast<double>(b) / static_cast<double>(a)) * 1000 / 60 << std::endl;
}