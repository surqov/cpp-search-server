#include <stack>
#include <iostream>

using namespace std;

template <typename Type>
class SortedStack {
public:
    void Push(const Type& element) {
        if (elements_.empty()) {
            elements_.push(element);
            ++push_count;
            return;
        }

        Type last_elem = elements_.top();
        if (last_elem < element) {
            elements_.pop();
            Push(element);
            elements_.push(last_elem);
            ++push_count;
        } else {
            elements_.push(element);
            ++push_count;
        }
    }

    int GetPushCount() {
        return push_count;
    }

    int GetIfCount(){
        return if_count;
    }
private:
    stack<Type> elements_;
    int push_count;
    int if_count;
};

int main() {
    SortedStack<int> stack_;
    for (int i = 1; i <= 100; ++i) {
        stack_.Push(i);
    }

    std::cout << stack_.GetIfCount() << std::endl;
    std::cout << stack_.GetPushCount() << std::endl;


}