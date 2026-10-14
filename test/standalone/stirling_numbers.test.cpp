// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/combinatorics/small_prime_permutation_cycle_counts.hpp"
#include "../../src/algorithm/math/combinatorics/stirling_numbers.hpp"

int main(){
    const SmallPrimePermutationCycleCounts permutations(7);
    assert(permutations.count(0, 0, 0) == 1);
    assert(permutations.count(4, 2, 0) == 3); // Three pairs of transpositions.
    assert(permutations.count(4, 2, 1) == 1); // 4 choices of fixed point, two 3-cycles: 8 mod 7.
    assert(permutations.count(5, 2, 0) == 6); // Choose a 2-cycle, order the 3-cycle: 20 mod 7.
    assert(permutations.count(4, 1, 1) == 0);
    assert(permutations.count(1000000000000000000LL, 1000000000000000000LL,
                              1000000000000000000LL) == 1);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int query_count;
    if(!(std::cin >> query_count)) return 1;
    while(query_count-- > 0){
        std::size_t order;
        std::cin >> order;
        const auto first =
            math::signed_stirling_numbers_first_kind<998244353>(order);
        const auto second =
            math::stirling_numbers_second_kind<998244353>(order);
        for(const auto& row: {first, second}){
            for(std::size_t index = 0; index < row.size(); ++index){
                if(index != 0) std::cout << ' ';
                std::cout << row[index];
            }
            std::cout << '\n';
        }
    }
}
