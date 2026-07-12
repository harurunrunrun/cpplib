#include <cassert>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../../src/algorithm/math/prime_sieve.hpp"

void test_is_prime_sqrt(){
    assert(!math::is_prime_sqrt(-1));
    assert(!math::is_prime_sqrt(0));
    assert(!math::is_prime_sqrt(1));
    assert(math::is_prime_sqrt(2));
    assert(math::is_prime_sqrt(99991));
    assert(!math::is_prime_sqrt(99991LL * 99991));
}

void test_sieve(){
    math::PrimeSieve<100> sieve(50);
    assert(sieve.size() == 50);
    assert(sieve.prime_count() == 15);
    assert(sieve.prime(0) == 2);
    assert(sieve.prime(14) == 47);
    assert((sieve.primes() == std::vector<int>{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47}));
    assert(sieve.is_prime(47));
    assert(!sieve.is_prime(48));
    assert(sieve.min_factor(1) == 1);
    assert(sieve.min_factor(49) == 7);
}

void test_factorize(){
    math::PrimeSieve<200> sieve(200);
    assert((sieve.factorize(1) == std::vector<std::pair<int,int>>{}));
    assert((sieve.factorize(60) == std::vector<std::pair<int,int>>{{2, 2}, {3, 1}, {5, 1}}));
    assert((sieve.factorize(128) == std::vector<std::pair<int,int>>{{2, 7}}));
    assert((sieve.factorize(199) == std::vector<std::pair<int,int>>{{199, 1}}));
}

void test_rebuild(){
    math::PrimeSieve<100> sieve(10);
    assert(sieve.prime_count() == 4);
    sieve.build(30);
    assert(sieve.prime_count() == 10);
    assert(sieve.prime(9) == 29);
}

void test_exceptions(){
    bool thrown = false;
    try{
        math::PrimeSieve<10> sieve(11);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    math::PrimeSieve<10> sieve(10);
    volatile int too_large = 11;
    thrown = false;
    try{
        (void)sieve.is_prime(too_large);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)sieve.factorize(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

inline void run_prime_sieve_standalone_check(){
    test_is_prime_sqrt();
    test_sieve();
    test_factorize();
    test_rebuild();
    test_exceptions();
}
