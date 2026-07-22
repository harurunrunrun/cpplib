// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/discrete_logarithm_fixed_mod

#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/number_theory/discrete_logarithm_fixed_mod.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::uint32_t prime, primitive_root, query_count;
    std::cin >> prime >> primitive_root >> query_count;
    math::FixedModDiscreteLogarithm logarithm(prime, primitive_root);
    while(query_count-- != 0){
        std::uint32_t value;
        std::cin >> value;
        std::cout << logarithm.log(value) << '\n';
    }
}
