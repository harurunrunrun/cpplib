// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>

#include "../../src/algorithm/string/maximum_consecutive_repetitions.hpp"

int main(){
    assert(maximum_consecutive_repetitions("") == 0);
    assert(maximum_consecutive_repetitions("a") == 1);
    assert(maximum_consecutive_repetitions("aaaaa") == 5);
    assert(maximum_consecutive_repetitions("abababab") == 4);
    assert(maximum_consecutive_repetitions("xababab") == 3);
    assert(maximum_consecutive_repetitions("abcabca") == 2);
    assert(maximum_consecutive_repetitions("abcdef") == 1);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        std::string text;
        std::cin >> text;
        std::cout << maximum_consecutive_repetitions(text) << '\n';
    }
}
