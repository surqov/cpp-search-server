#include "log_duration.h"

#include <iostream>

using namespace std;
using namespace std::literals;

class StreamUntier {
public:
    StreamUntier(istream& stream) : stream_(stream) {
        tied_before_ = stream.tie(nullptr);
    }
    
    ~StreamUntier(){
       stream_.tie(tied_before_); 
    }

private:
    ostream* tied_before_;
    istream& stream_;
};

int main() {
    LOG_DURATION("\\n with tie"s);

    StreamUntier guard(cin);
    int i;
    while (cin >> i) {
        cout << i * i << "\n"s;
    }

    return 0;
}