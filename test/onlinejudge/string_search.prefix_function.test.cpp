// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_14_B

#include <iostream>
#include <string>

#include "../../src/algorithm/string/prefix_function.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text, pattern;
    std::cin >> text >> pattern;
    for(int position: kmp_search(text, pattern)){
        std::cout << position << '\n';
    }
}
