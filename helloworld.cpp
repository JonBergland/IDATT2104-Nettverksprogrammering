#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {

    vector<string> greeting = {"Hello", "World"};

    for(const auto &word: greeting) {
        cout << word << ", ";
    }
    cout << endl;


}
