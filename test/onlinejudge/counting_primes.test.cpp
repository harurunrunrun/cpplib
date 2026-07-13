// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/counting_primes

#include <iostream>
#include "../../src/algorithm/math/prime_counting.hpp"

int main(){
    long long n;
    std::cin >> n;
    std::cout << math::prime_count(n) << '\n';
}
