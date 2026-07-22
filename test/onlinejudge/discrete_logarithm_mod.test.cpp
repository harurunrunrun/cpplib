// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/discrete_logarithm_mod

#include <iostream>
#include "../../src/algorithm/math/number_theory/discrete_log.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    while(test_count--){
        long long x, y, modulus;
        std::cin >> x >> y >> modulus;
        std::cout << math::discrete_log(x, y, modulus) << '\n';
    }
}
