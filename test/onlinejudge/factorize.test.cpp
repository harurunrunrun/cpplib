// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/factorize

#include <iostream>
#include "../../src/algorithm/math/prime_factorization.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        unsigned long long value;
        std::cin >> value;
        const auto factors = math::factorize_pollard_rho(value);
        int count = 0;
        for(auto [prime, exponent]: factors){
            (void)prime;
            count += exponent;
        }
        std::cout << count;
        for(auto [prime, exponent]: factors){
            while(exponent-- > 0) std::cout << ' ' << prime;
        }
        std::cout << '\n';
    }
}
