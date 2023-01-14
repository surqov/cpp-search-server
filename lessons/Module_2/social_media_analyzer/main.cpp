#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <deque>
#include <future>
#include <execution>
#include <algorithm>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats& other) {
            std::for_each(std::execution::par,
                         std::begin(other.word_frequences),
                         std::end(other.word_frequences),
                         [this](const auto& pair){
                            word_frequences[pair.first] += pair.second;
                         });
    }
    
    int& operator[](const string& word) {
        return word_frequences[word];
    }
};

std::vector<std::string> SplitIntoWords(const string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

bool only_letters(std::string const &str) {
    auto it = std::find_if(str.begin(), str.end(), [](char const &c) {
        return !std::isalpha(c);
    });
    return it == str.end();
}

using KeyWords = set<string, std::less<>>;

Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {
    Stats result_;
    std::deque<std::string> words_deq;
    
    future<void> read_split_write_ = async([&input, &words_deq, &key_words]{
        std::string temp;
        while (std::getline(input, temp)) {
            for (const std::string& w : SplitIntoWords(temp)) {
                if (key_words.count(w) && only_letters(w)) {
                    words_deq.emplace_back(w);
                }
            }
        }
    });
    
    while(!words_deq.size()) {
        std::cout << ""s;
    }
    
    future<void> count_ = async([&result_, &words_deq]{
        std::cout << "";
        while (words_deq.size()) {
            ++result_[words_deq.front()];
            words_deq.pop_front();
        }
    });
    
    read_split_write_.get();
    count_.get();
    
    return result_;
}

int main() {
    const KeyWords key_words = {"yangle", "rocks", "sucks", "all"};
                                //6          2        1       0
    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    for (const auto& [word, frequency] : ExploreKeyWords(key_words, ss).word_frequences) {
        cout << word << " " << frequency << endl;
    }

    return 0;
}