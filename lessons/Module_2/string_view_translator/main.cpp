#include "my_assert.h"
#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;

class Translator {
public:
    Translator() : base_forward_({}), base_backward_({}) {}
    
    void Add(string_view source, string_view target) {
        base_forward_[source] = target;
        base_backward_[target] = source;
    }
    
    string_view TranslateForward(string_view source) const {
        try {
            return base_forward_.at(source);
        } catch (const out_of_range& e) {
            return {};
        }
    }

    string_view TranslateBackward(string_view target) const {
        try {
            return base_backward_.at(target);
        } catch (const out_of_range& e) {
            return {};
        }
    }

private:
    map<string_view, string_view> base_forward_;
    map<string_view, string_view> base_backward_;
};

void TestSimple() {
    Translator translator;
    translator.Add(string("okno"s), string("window"s));
    translator.Add(string("stol"s), string("table"s));

    assert(translator.TranslateForward("okno"s) == "window"s);
    assert(translator.TranslateBackward("table"s) == "stol"s);
    assert(translator.TranslateForward("table"s) == ""s);
}

int main() {
    TestSimple();
    return 0;
} 