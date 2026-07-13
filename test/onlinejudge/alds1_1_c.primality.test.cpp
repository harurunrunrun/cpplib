#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_1_C"

#include <iostream>

#include "../../src/algorithm/math/prime_factorization.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int count;
    std::cin >> count;
    int answer = 0;
    while(count--){
        unsigned long long value;
        std::cin >> value;
        answer += math::is_prime_miller_rabin(value);
    }
    std::cout << answer << '\n';
}
