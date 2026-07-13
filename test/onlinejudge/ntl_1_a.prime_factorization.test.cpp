#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/NTL_1_A"

#include <iostream>

#include "../../src/algorithm/math/prime_factorization.hpp"

int main(){
    unsigned long long value;
    std::cin >> value;
    std::cout << value << ':';
    for(auto [prime, exponent]: math::factorize_pollard_rho(value)){
        while(exponent--) std::cout << ' ' << prime;
    }
    std::cout << '\n';
}
