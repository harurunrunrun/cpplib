// competitive-verifier: PROBLEM https://www.spoj.com/problems/AMR11E/
#include <algorithm>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/nth_three_prime_factors.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    std::vector<int> ranks(static_cast<std::size_t>(tests));
    for(int& rank: ranks) std::cin >> rank;
    const math::ThreeDistinctPrimeFactorsTable table(
        *std::max_element(ranks.begin(), ranks.end())
    );
    for(const int rank: ranks) std::cout << table.value(rank) << '\n';
}
