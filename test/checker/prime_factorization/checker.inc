#include <cassert>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../../src/algorithm/math/prime_factorization.hpp"

using math::u64;

void test_miller_rabin(){
    assert(!math::is_prime_miller_rabin(0));
    assert(!math::is_prime_miller_rabin(1));
    assert(math::is_prime_miller_rabin(2));
    assert(math::is_prime_miller_rabin(3));
    assert(!math::is_prime_miller_rabin(4));
    assert(math::is_prime_miller_rabin(1000000007ULL));
    assert(math::is_prime_miller_rabin(2305843009213693951ULL));
    assert(!math::is_prime_miller_rabin(341550071728321ULL));
    assert(!math::is_prime_miller_rabin(3825123056546413051ULL));

    std::mt19937_64 rng(12345);
    assert(math::is_prime_miller_rabin_random(1000000007ULL, 8, rng));
    assert(!math::is_prime_miller_rabin_random(1000000007ULL * 1000000009ULL, 8, rng));
}

void test_factorize(){
    assert((math::factorize_pollard_rho(1) == std::vector<std::pair<u64,int>>{}));
    assert((math::factorize_pollard_rho(60) == std::vector<std::pair<u64,int>>{{2, 2}, {3, 1}, {5, 1}}));
    assert((math::factorize_pollard_rho(600851475143ULL) == std::vector<std::pair<u64,int>>{{71, 1}, {839, 1}, {1471, 1}, {6857, 1}}));
    assert((math::factorize_pollard_rho(1000000007ULL * 1000000009ULL) == std::vector<std::pair<u64,int>>{{1000000007ULL, 1}, {1000000009ULL, 1}}));
    assert((math::factorize_pollard_rho(2305843009213693951ULL) == std::vector<std::pair<u64,int>>{{2305843009213693951ULL, 1}}));
}

void test_phi(){
    assert(math::euler_phi(1) == 1);
    assert(math::euler_phi(2) == 1);
    assert(math::euler_phi(36) == 12);
    assert(math::euler_phi(1000000007ULL) == 1000000006ULL);
    assert(math::euler_phi(1000000007ULL * 1000000009ULL) == 1000000006ULL * 1000000008ULL);
}

void test_exceptions(){
    bool thrown = false;
    try{
        (void)math::factorize_pollard_rho(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)math::pollard_rho(1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    std::mt19937_64 rng(1);
    thrown = false;
    try{
        (void)math::is_prime_miller_rabin_random(101, -1, rng);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

inline void run_prime_factorization_standalone_check(){
    test_miller_rabin();
    test_factorize();
    test_phi();
    test_exceptions();
}
