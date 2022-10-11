#include <cassert>
#include <iostream>
#include <string>

using namespace std;

const string& FindMinStr(const string& a, const string& b, const string& c) {
    if (a <= b && a <= c) {
        return a;
    } else if (b <= a && b <= c) {
        return b;
    }
    return c;
}

void TestFindMinStr() {
    assert(FindMinStr("alpha"s, "beta"s, "gamma"s) == "alpha"s);
    assert(FindMinStr("beta"s, "alpha"s, "gamma"s) == "alpha"s);
    assert(FindMinStr("gamma"s, "beta"s, "alpha"s) == "alpha"s);

    assert(FindMinStr("alpha"s, "beta"s, "alpha"s) == "alpha"s);
    assert(FindMinStr("beta"s, "alpha"s, "alpha"s) == "alpha"s);
    assert(FindMinStr("alpha"s, "alpha"s, "beta"s) == "alpha"s);

    assert(FindMinStr("alpha"s, "alpha"s, "alpha"s) == "alpha"s);

    assert(FindMinStr(""s, "alpha"s, "beta"s) == ""s);
    assert(FindMinStr("beta"s, ""s, "alpha"s) == ""s);
    assert(FindMinStr("beta"s, "alpha"s, ""s) == ""s);

    cout << "TestFindMinStr is OK"s << endl;
}

int main() {
    TestFindMinStr();

    string a, b, c;

    cin >> a >> b >> c;

    cout << FindMinStr(a, b, c) << endl;
} 