// competitive-verifier: PROBLEM https://www.spoj.com/problems/TDPRIMES/
#include <iostream>

#include "../../src/algorithm/math/sampled_primes.hpp"

int main(){
    for(const int prime: math::sampled_primes(100'000'000)){
        std::cout << prime << '\n';
    }
}
