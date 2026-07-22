// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/string/automata/minimum_binary_substring_mismatches.hpp"

void self_test(){
    assert(minimum_binary_substring_mismatches("", "") == 0);
    assert(minimum_binary_substring_mismatches("0101", "") == 0);
    assert(minimum_binary_substring_mismatches("0001", "101") == 1);
    assert(minimum_binary_substring_mismatches("0101010", "1010101") == 7);
    assert(minimum_binary_substring_mismatches("101101", "110") == 0);
    assert(minimum_binary_substring_mismatches("00000", "111") == 3);

    bool thrown = false;
    try{
        static_cast<void>(minimum_binary_substring_mismatches("01", "010"));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        static_cast<void>(minimum_binary_substring_mismatches("012", "01"));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    std::string text, pattern;
    if(!(std::cin >> text >> pattern)) return 0;
    std::cout << minimum_binary_substring_mismatches(text, pattern) << '\n';
}
