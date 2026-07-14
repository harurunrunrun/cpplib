// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DPL_1_E

#include <iostream>
#include <string>

#include "../../src/algorithm/string/edit_distance.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string a, b;
    std::cin >> a >> b;
    std::cout << edit_distance(a, b) << '\n';
}
