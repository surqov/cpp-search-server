// Прочитай биты INT как биты FLOAT
#include <iostream>

int main() {
    int* x = new int(5);
    std::cout << *x << std::endl;
    float* y = reinterpret_cast<float*>(x);
    std::cout << *y << std::endl;

    int z = 15;
    std::cout << z << std::endl;
    float& z_cast = (float)(z);
    std::cout << z_cast << std::endl;

    return 1;
}