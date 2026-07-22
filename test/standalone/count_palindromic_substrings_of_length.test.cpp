// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>

#include "../../src/algorithm/string/palindrome/count_palindromic_substrings_of_length.hpp"

int main(){
    assert(count_palindromic_substrings_of_length("", 1) == 0);
    assert(count_palindromic_substrings_of_length("a", 1) == 1);
    assert(count_palindromic_substrings_of_length("aaaa", 2) == 3);
    assert(count_palindromic_substrings_of_length("ababa", 3) == 3);
    assert(count_palindromic_substrings_of_length("abc", 0) == 0);
    assert(count_palindromic_substrings_of_length("abc", 4) == 0);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        int length;
        std::string text;
        std::cin >> length >> text;
        std::cout << count_palindromic_substrings_of_length(text, length) << '\n';
    }
}
