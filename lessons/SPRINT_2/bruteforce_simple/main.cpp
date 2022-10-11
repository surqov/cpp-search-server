#include <iostream>
#include <string>
#include <sstream>

using namespace std;

template <typename F>
string BruteForce(F check) {
    for (char c1 = 'A'; c1 <= 'Z'; ++c1) {
        for (char c2 = 'A'; c2 <= 'Z'; ++c2) {
            for (char c3 = 'A'; c3 <= 'Z'; ++c3) {
                for (char c4 = 'A'; c4 <= 'Z'; ++c4) {
                    for (char c5 = 'A'; c5 <= 'Z'; ++c5) {
                        std::ostringstream ss;
                        ss << c1 << c2 << c3 << c4 << c5;
                        std::string s1 = ss.str();
                        if (check(s1)) {
                            return s1;
                        }
                    }
                }
            }
        }
    }
    return ""s;
}

int main() {
    string pass = "ARTUP"s;
    auto check = [pass](const string& s) {
        return s == pass;
    };
    cout << BruteForce(check) << endl;
}