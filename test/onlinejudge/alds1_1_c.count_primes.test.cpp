// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_1_C

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/number_theory/count_primes.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int count;
    std::cin >> count;
    std::vector<unsigned long long> values(static_cast<std::size_t>(count));
    for(auto& value: values) std::cin >> value;
    std::cout << math::count_primes(values) << '\n';
}
