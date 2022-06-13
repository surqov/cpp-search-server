// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь: 271 
//
// Закомитьте изменения и отправьте их в свой репозиторий.
#include <iostream>
using namespace std;

int main() {
    int count = 0;
    int the_number = 3;
    for (int i = 0; i < 1000; ++i) {
        if ((i < 10) 
            && (i / the_number == 1) && (i % the_number == 0)) {
            ++count;   
        } else if ((i >= 10) && (i < 100) 
                    && ((i / 10 == the_number) || (i % 10 == the_number))) {
            ++count;
        } else if ((i >= 100) 
                    && ((i / 100 == the_number) || (i % 100 / 10 == the_number) || (i % 10 == the_number))) {
            ++count;     
        }
    }
    cout << count << endl;
}
