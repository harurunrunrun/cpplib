// competitive-verifier: STANDALONE

#include <cassert>
#include "../../src/algorithm/math/prime_counting.hpp"

bool is_prime_naive(int n){
    if(n <= 1) return false;
    for(int d = 2; d * d <= n; d++){
        if(n % d == 0) return false;
    }
    return true;
}

int main(){
    math::PrimeCounting<1000> pc(1000);
    int count = 0;
    for(int n = 0; n <= 1000; n++){
        if(is_prime_naive(n)) count++;
        assert(pc.is_prime(n) == is_prime_naive(n));
        assert(pc.count_primes(n) == count);
        assert(pc.count_primes_less(n + 1) == count);
    }
    assert(pc.count_primes(10) == 4);
    assert(pc.count_primes_less(10) == 4);
    assert(pc.count_primes_less(11) == 4);
    assert(pc.count_primes(11) == 5);
}
