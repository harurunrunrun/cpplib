// competitive-verifier: PROBLEM https://www.spoj.com/problems/LCS2/

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/longest_common_substring_multiple_length.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<std::string> texts;
    for(std::string text; std::cin >> text;) texts.push_back(std::move(text));
    if(texts.empty()) return 0;

    std::cout << longest_common_substring_multiple_length<100000>(texts) << '\n';
}
