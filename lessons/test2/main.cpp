#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <chrono>
#include <thread>

using namespace std::literals;

class House {
    public:
        explicit House() = default;

        explicit House(std::string name, int num) 
            : name_(name), num_(num){

            }

        std::string& GetName() {
            return name_;
        }

        int GetNum() {
            return num_;
        }

        House& operator=(int num) {
            num_ = num;
            return *this;
        }

        House& operator=(std::string name) {
            name_ = name;
            return *this;
        }     

    private:
        std::string name_;
        int num_ = 1;
};

int main() {
    House new_one("Revolucionnaya"s, 1);
    std::cout << new_one.GetName() << std::endl;
    new_one = "IKRA"s;
    std::cout << new_one.GetName() << std::endl;

    for (int i = 0; i < 1000; ++i) {
        std::cout << std::chrono::system_clock::now() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}