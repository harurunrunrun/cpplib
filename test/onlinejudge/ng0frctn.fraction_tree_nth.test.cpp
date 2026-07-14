// competitive-verifier: PROBLEM https://www.spoj.com/problems/NG0FRCTN/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/fraction_tree_nth.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    for(std::uint64_t index; std::cin >> index && index != 0;){
        const auto [numerator, denominator] = math::fraction_tree_nth(index);
        std::cout << numerator << '/' << denominator << '\n';
    }
}
