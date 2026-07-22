// competitive-verifier: PROBLEM https://www.spoj.com/problems/NG0FRCTN/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/combinatorics/calkin_wilf_nth_fraction.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    for(std::uint64_t index; std::cin >> index && index != 0;){
        const auto [numerator, denominator] = math::calkin_wilf_nth_fraction(index);
        std::cout << numerator << '/' << denominator << '\n';
    }
}
