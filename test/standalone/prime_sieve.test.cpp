// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/number_theory/prime_sieve.hpp"

void test_is_prime_sqrt(){
    assert(!math::is_prime_sqrt(-1));
    assert(!math::is_prime_sqrt(0));
    assert(!math::is_prime_sqrt(1));
    assert(math::is_prime_sqrt(2));
    assert(math::is_prime_sqrt(99991));
    assert(!math::is_prime_sqrt(99991LL * 99991));
    assert(!math::is_prime_sqrt(9'223'372'036'854'775'807LL));

    constexpr long long maximum = 9'223'372'036'854'775'807LL;
    static_assert(math::prime_sieve_internal::square_less_equal(
        3'037'000'499LL, maximum));
    static_assert(!math::prime_sieve_internal::square_less_equal(
        3'037'000'500LL, maximum));
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

int main(){
    int limit, q;
    if(std::cin >> limit >> q){
        math::PrimeSieve<200000> sieve(limit);
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "COUNT"){
                std::cout << sieve.prime_count() << '\n';
                continue;
            }
            long long value;
            std::cin >> value;
            if(type == "BOUND"){
                long long limit_value;
                std::cin >> limit_value;
                std::cout << math::prime_sieve_internal::square_less_equal(
                    value, limit_value) << '\n';
            }else if(type == "SQRT"){
                std::cout << math::is_prime_sqrt(value) << '\n';
            }else if(type == "IS"){
                std::cout << sieve.is_prime(static_cast<int>(value)) << '\n';
            }else if(type == "MIN"){
                std::cout << sieve.min_factor(static_cast<int>(value)) << '\n';
            }else if(type == "KTH"){
                std::cout << sieve.prime(static_cast<int>(value)) << '\n';
            }else if(type == "FACT"){
                const auto factors = sieve.factorize(static_cast<int>(value));
                std::cout << factors.size();
                for(auto [prime, exponent]: factors){
                    std::cout << ' ' << prime << ' ' << exponent;
                }
                std::cout << '\n';
            }
        }
        return 0;
    }

    test_is_prime_sqrt();
    test_sieve();
    test_factorize();
    test_rebuild();
    test_exceptions();
}
