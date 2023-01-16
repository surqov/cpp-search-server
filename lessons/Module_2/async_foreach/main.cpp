#include <algorithm>
#include <execution>
#include <iostream>
#include <list>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <future>

#include "log_duration.h"

const int BASIC_STREAM_COUNT = 5;

using namespace std;

string GenerateWord(mt19937& generator, int max_length) {
    const int length = uniform_int_distribution(1, max_length)(generator);
    string word;
    word.reserve(length);
    for (int i = 0; i < length; ++i) {
        word.push_back(uniform_int_distribution('a', 'z')(generator));
    }
    return word;
}

template <template <typename> typename Container>
Container<string> GenerateDictionary(mt19937& generator, int word_count, int max_length) {
    vector<string> words;
    words.reserve(word_count);
    for (int i = 0; i < word_count; ++i) {
        words.push_back(GenerateWord(generator, max_length));
    }
    return Container(words.begin(), words.end());
}

struct Reverser {
    void operator()(string& value) const {
        reverse(value.begin(), value.end());
    }
};

template <typename Container, typename Function>
void Test(string_view mark, Container keys, Function function) {
    LOG_DURATION(mark);
    function(keys, Reverser{});
}

#define TEST(function) Test(#function, keys, function<remove_const_t<decltype(keys)>, Reverser>)

template <typename ForwardRange, typename Function>
void ForEach(ForwardRange& range, Function function) {
    size_t steps_ = range.size() > BASIC_STREAM_COUNT
                                    ? range.size() / BASIC_STREAM_COUNT
                                    : 1; // каждые n шагов - запускаем поток
    size_t curr_step_ = 1; // текущий шаг = 0
    auto begin_ = range.begin(); // итератор начала текущего чанка
    [[maybe_unused]] auto end_ = range.end(); // конец контейнера
    vector<future<void>> results_; // вектор потоков
    size_t n = 0; // кол-во запусков
    
    for (auto it = range.begin(); it != range.end(); ++it, ++curr_step_) {
        if (range.size() / steps_ - 1 == n) {
            results_.push_back(
                async([begin_, end_, function]{
                    for_each(execution::par, begin_, end_, function);
                })
            );
            break;
        } else if (curr_step_ % steps_ == 0) {
            results_.push_back(
                async([begin_, it, function]{
                    for_each(execution::par, begin_, it, function);
                })
            );
            begin_ = it;
            ++n;
        }
    }
}

int main() {
    // для итераторов с произвольным доступом тоже должно работать
    vector<string> strings = {"1cat", "1dog", "1code", "1node", "1asd", "1qwerty", "1lolkek"};

    ForEach(strings, [](string& s) {
        reverse(s.begin(), s.end());
    });

    for (string_view s : strings) {
        cout << s << " ";
    }
    cout << endl;
    // вывод: tac god edoc

    mt19937 generator;
    const auto keys = GenerateDictionary<list>(generator, 50'000, 5'000);

    TEST(ForEach);

    return 0;
}