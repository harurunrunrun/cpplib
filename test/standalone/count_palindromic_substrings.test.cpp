// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>

#include "../../src/algorithm/string/palindrome/count_palindromic_substrings.hpp"

int main(){
    assert(count_palindromic_substrings("") == 0);
    assert(count_palindromic_substrings("a") == 1);
    assert(count_palindromic_substrings("aaa") == 6);
    assert(count_palindromic_substrings("abac") == 5);
    assert(count_palindromic_substrings("Aa") == 2);
    assert(count_palindromic_substrings("malayalam") == 15);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string text;
        std::cin >> text;
        std::cout << count_palindromic_substrings(text) << '\n';
    }
}
