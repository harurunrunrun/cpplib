// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include "../../src/algorithm/math/prime_counting.hpp"

bool is_prime_naive(int n){
    if(n <= 1) return false;
    for(int d = 2; d * d <= n; d++){
        if(n % d == 0) return false;
    }
    return true;
}

int main(){
    int limit, q;
    if(std::cin >> limit >> q){
        math::PrimeCounting<1000000> counting(limit);
        while(q--){
            std::string type;
            int n;
            std::cin >> type >> n;
            if(type == "IS") std::cout << counting.is_prime(n) << '\n';
            if(type == "COUNT") std::cout << counting.count_primes(n) << '\n';
            if(type == "LESS") std::cout << counting.count_primes_less(n) << '\n';
        }
        return 0;
    }

    math::PrimeCounting<1000> pc(1000);
    assert(pc.size() == 1000);
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

    pc.build(20);
    assert(pc.size() == 20);
    assert(pc.count_primes(20) == 8);
    assert(pc.count_primes_less(20) == 8);

    bool thrown = false;
    try{
        pc.build(1001);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
