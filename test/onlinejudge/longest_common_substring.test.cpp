// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/longest_common_substring

#include <iostream>
#include <string>
#include "../../src/algorithm/string/suffix/longest_common_substring.hpp"

int main(){
    std::string first, second;
    std::cin >> first >> second;
    const auto result = longest_common_substring(first, second);
    std::cout << result.first_pos << ' ' << result.first_pos + result.length << ' '
              << result.second_pos << ' ' << result.second_pos + result.length << '\n';
}
