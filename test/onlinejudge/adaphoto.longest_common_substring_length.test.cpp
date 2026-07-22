// competitive-verifier: PROBLEM https://www.spoj.com/problems/ADAPHOTO/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/suffix/longest_common_substring_length.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string first, second;
    if(!(std::cin >> first >> second)) return 0;
    std::cout << longest_common_substring_length(first, second) << '\n';
}
