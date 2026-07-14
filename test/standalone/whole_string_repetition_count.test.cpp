// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/whole_string_repetition_count.hpp"

int main(){
    assert(whole_string_repetition_count(std::string()) == 0);
    assert(whole_string_repetition_count(std::string("a")) == 1);
    assert(whole_string_repetition_count(std::string("ababab")) == 3);
    assert(whole_string_repetition_count(std::string("ababa")) == 1);
    assert(whole_string_repetition_count(std::vector<int>{1, 2, 1, 2}) == 2);

    std::string text;
    while(std::cin >> text && text != "*"){
        std::cout << whole_string_repetition_count(text) << '\n';
    }
}
