// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/q_binomial_coefficient_prime_mod

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/combinatorics/q_binomial_prime_mod.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    std::uint32_t modulus, q;
    std::cin >> query_count >> modulus >> q;
    std::vector<std::pair<std::size_t, std::size_t>> queries(query_count);
    std::size_t maximum_n = 0;
    for(auto& [n, k] : queries){
        std::cin >> n >> k;
        if(n > maximum_n) maximum_n = n;
    }

    const QBinomialPrimeMod solver(modulus, q, maximum_n);
    for(const auto& [n, k] : queries){
        std::cout << solver.binomial(n, k) << '\n';
    }
}
