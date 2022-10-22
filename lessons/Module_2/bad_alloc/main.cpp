#include <iostream>
#include <new>
#include <string>

using namespace std;

int main() {
    {
        Octopus octopus;
        // Мы просто хотели ещё одного осьминога
        Octopus octopus1 = octopus;
        cout << "Tentacle id="s << octopus1.GetTentacle(3).GetId() << endl;
    }
    cout << "OK"s << endl;
} 