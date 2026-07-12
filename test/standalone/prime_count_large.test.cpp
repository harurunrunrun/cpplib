// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/prime_counting.hpp"

int main(){
    int query_count;
    if(std::cin >> query_count){
        while(query_count--){
            long long n;
            std::cin >> n;
            std::cout << math::prime_count(n) << '\n';
        }
        return 0;
    }

    assert(math::prime_count(0) == 0);
    assert(math::prime_count(1) == 0);
    assert(math::prime_count(10) == 4);
    assert(math::prime_count(1000000) == 78498);
    assert(math::count_primes(100000000) == 5761455);

    bool thrown = false;
    try{
        (void)math::prime_count(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
