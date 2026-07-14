// competitive-verifier: PROBLEM https://www.spoj.com/problems/PLD/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/count_palindromic_substrings_of_length.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int length;
    std::string text;
    if(!(std::cin >> length >> text)) return 0;
    std::cout << count_palindromic_substrings_of_length(text, length) << '\n';
}
