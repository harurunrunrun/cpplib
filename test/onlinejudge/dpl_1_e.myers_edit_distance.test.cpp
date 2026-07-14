// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DPL_1_E

#include <iostream>
#include <string>

#include "../../src/algorithm/string/myers_edit_distance.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string first, second;
    std::cin >> first >> second;
    std::cout <<
        myers_bit_parallel_edit_distance(first, second) <<
        '\n';
}
