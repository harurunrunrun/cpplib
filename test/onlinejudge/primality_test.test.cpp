// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/primality_test

#include <iostream>
#include "../../src/algorithm/math/number_theory/prime_factorization.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        unsigned long long value;
        std::cin >> value;
        std::cout << (math::is_prime_miller_rabin(value) ? "Yes" : "No") << '\n';
    }
}
