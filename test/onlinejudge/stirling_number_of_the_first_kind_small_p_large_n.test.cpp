// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/stirling_number_of_the_first_kind_small_p_large_n

#include <iostream>
#include "../../src/algorithm/math/combinatorics/small_prime_stirling_numbers.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int queries, prime;
    std::cin >> queries >> prime;
    SmallPrimeStirlingNumbers table(prime, true, false);
    while(queries--){
        long long n, k;
        std::cin >> n >> k;
        std::cout << table.signed_first(n, k) << '\n';
    }
}
