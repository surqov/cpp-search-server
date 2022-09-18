#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

template <typename RandomIt>
pair<RandomIt, RandomIt> FindStartsWith(RandomIt range_begin, RandomIt range_end, const char prefix) {
        pair<RandomIt, RandomIt> result;
        bool check = true;
        for (auto it = range_begin; it != range_end; ++it) {
            if ((*it)[0] == prefix && check) {
                result.first = it;
                check = false;
            } else if ((*it)[0] == prefix) {
                result.second = it+1;
            }
        }
        if (check) {
            auto is_position_fit = [prefix](auto it){return (it[0] > prefix);};
            auto position = find_if(range_begin, range_end, is_position_fit);
            if (position != range_end) {
                result.first = position;
                result.second = position;
            } else {
               result.first = range_end;
               result.second = range_end; 
            }
        }
        
        return result;
}

int main() {
    const vector<string> sorted_strings = {"m", "n"};

    const auto m_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'm');
    for (auto it = m_result.first; it != m_result.second; ++it) {
        cout << *it << " ";
    }
    cout << endl;

    const auto p_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'p');
    cout << (p_result.first - begin(sorted_strings)) << " " << (p_result.second - begin(sorted_strings)) << endl;

    const auto z_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'z');
    cout << (z_result.first - begin(sorted_strings)) << " " << (z_result.second - begin(sorted_strings)) << endl;

    vector<string> sorted_strings2;
    string str;
    for (char ch = 'a'; ch < 'z'; ++ch) {
        str.append("a" + std::string(1,ch));
        sorted_strings2.push_back(str);
        str.clear();
    }
    sorted_strings2.push_back("b");
    sorted_strings2.push_back("bb");
    sorted_strings2.push_back("bbb");
    const auto result = FindStartsWith(begin(sorted_strings2), end(sorted_strings2), 'a');
    auto st = result.first - sorted_strings2.begin();
    auto en = result.second - sorted_strings2.begin();
    std::cout << st << ' ' << en << '\n';

    return 0;
} 