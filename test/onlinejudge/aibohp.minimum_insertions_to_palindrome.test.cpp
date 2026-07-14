// competitive-verifier: PROBLEM https://www.spoj.com/problems/AIBOHP/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/minimum_insertions_to_palindrome.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::string text;
        std::cin >> text;
        std::cout << minimum_insertions_to_palindrome(text) << '\n';
    }
}
