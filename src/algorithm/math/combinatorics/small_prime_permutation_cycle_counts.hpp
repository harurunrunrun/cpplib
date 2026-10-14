#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_SMALL_PRIME_PERMUTATION_CYCLE_COUNTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_SMALL_PRIME_PERMUTATION_CYCLE_COUNTS_HPP_INCLUDED

#include "binomial_tables.hpp"
#include "small_prime_stirling_numbers.hpp"

// Counts permutations with a prescribed number of cycles and fixed points.
// Precomputation: O(p^2). Query: O((k - fixed + 1) log_p(n + 1)).
class SmallPrimePermutationCycleCounts{
    int prime_;
    SmallPrimeStirlingNumbers stirling_;
    math::BinomialPrimeTable binomial_;

public:
    explicit SmallPrimePermutationCycleCounts(int prime)
        : prime_(prime), stirling_(prime, true, false),
          binomial_(static_cast<unsigned>(prime), static_cast<unsigned>(prime - 1)){}

    // n, k and fixed may be as large as LLONG_MAX. Impossible inputs return 0.
    int count(long long n, long long k, long long fixed) const{
        if(n < 0 || k < 0 || fixed < 0 || fixed > k || k > n) return 0;
        const long long remaining = n - fixed;
        const long long cycles = k - fixed;
        if(cycles > remaining / 2 || (remaining != 0 && cycles == 0)) return 0;
        const int choose_fixed = static_cast<int>(binomial_.binomial(n, fixed));
        if(choose_fixed == 0) return 0;
        int answer = 0;
        for(long long removed = 0; removed <= cycles; ++removed){
            const int choose_removed = static_cast<int>(binomial_.binomial(remaining, removed));
            const int term = choose_removed * stirling_.unsigned_first(
                remaining - removed, cycles - removed) % prime_;
            if(removed & 1){
                answer -= term;
                if(answer < 0) answer += prime_;
            }else{
                answer += term;
                if(answer >= prime_) answer -= prime_;
            }
        }
        return choose_fixed * answer % prime_;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_SMALL_PRIME_PERMUTATION_CYCLE_COUNTS_HPP_INCLUDED
