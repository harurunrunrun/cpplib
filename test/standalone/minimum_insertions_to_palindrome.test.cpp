// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/minimum_insertions_to_palindrome.hpp"

int main(){
    assert(minimum_insertions_to_palindrome(std::string()) == 0);
    assert(minimum_insertions_to_palindrome(std::string("a")) == 0);
    assert(minimum_insertions_to_palindrome(std::string("abba")) == 0);
    assert(minimum_insertions_to_palindrome(std::string("fft")) == 1);
    assert(minimum_insertions_to_palindrome(std::string("abc")) == 2);
    assert(minimum_insertions_to_palindrome(std::string("Aa")) == 1);
    assert(minimum_insertions_to_palindrome(std::vector<int>{1, 2, 1}) == 0);
    assert(minimum_insertions_to_palindrome(std::vector<int>{1, 2, 3}) == 2);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string text;
        std::cin >> text;
        std::cout << minimum_insertions_to_palindrome(text) << '\n';
    }
}
