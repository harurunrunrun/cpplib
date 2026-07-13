// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/wildcard_pattern_matching

#include <iostream>
#include <string>

#include "../../src/algorithm/string/wildcard_pattern_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    std::string pattern;
    std::cin >> text >> pattern;
    const auto matches = wildcard_pattern_matching(text, pattern);
    for(bool match: matches) std::cout << static_cast<int>(match);
    std::cout << '\n';
}
