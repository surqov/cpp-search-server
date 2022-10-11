#include "log_duration.h"

#include <fstream>
#include <string>

using namespace std;

void test_endl(){
    ofstream out_file("ballad.txt"s);
    for (int i = 0; i < 50000; ++i) {
        out_file << "С любимыми не расставайтесь"s << endl;
    }
}

void test_n(){
    ofstream out_file("ballad.txt"s);
    for (int i = 0; i < 50000; ++i) {
        out_file << "С любимыми не расставайтесь \n"s;
    }
}

int main() {
    {
        LOG_DURATION("endl");
        test_endl();
    }

    {
        LOG_DURATION("\\n");
        test_n();
    }

} 