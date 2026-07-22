// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc196/tasks/abc196_f

#include <iostream>
#include <string>

#include "../../src/algorithm/string/automata/minimum_binary_substring_mismatches.hpp"

int main(){
    std::string s, t;
    std::cin >> s >> t;
    std::cout << minimum_binary_substring_mismatches(s, t) << '\n';
}
