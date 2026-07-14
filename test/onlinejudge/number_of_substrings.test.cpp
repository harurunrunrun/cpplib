// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/number_of_substrings

#include <iostream>
#include <string>

#include "../../src/algorithm/string/suffix_array.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    std::cin >> text;
    std::cout << number_of_distinct_substrings(text) << '\n';
}
