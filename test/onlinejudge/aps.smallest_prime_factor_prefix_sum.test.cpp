// competitive-verifier: PROBLEM https://www.spoj.com/problems/APS/
#include <algorithm>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/number_theory/smallest_prime_factor_prefix_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    std::vector<int> queries(static_cast<std::size_t>(tests));
    for(int& n: queries) std::cin >> n;
    const math::SmallestPrimeFactorPrefixSum table(
        *std::max_element(queries.begin(), queries.end())
    );
    for(const int n: queries) std::cout << table.value(n) << '\n';
}
