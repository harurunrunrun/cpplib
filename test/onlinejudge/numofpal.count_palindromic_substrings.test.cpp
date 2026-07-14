// competitive-verifier: PROBLEM https://www.spoj.com/problems/NUMOFPAL/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/count_palindromic_substrings.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if(!(std::cin >> text)) return 0;
    std::cout << count_palindromic_substrings(text) << '\n';
}
