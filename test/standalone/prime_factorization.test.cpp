// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/prime_factorization.hpp"

using math::u64;

void test_miller_rabin(){
    constexpr u64 a = 18'446'744'073'709'551'000ULL;
    constexpr u64 b = 18'446'744'073'709'550'000ULL;
    constexpr u64 modulus = 1'000'000'007ULL;
    assert(
        math::mul_mod_u64(a, b, modulus) ==
        static_cast<u64>(static_cast<math::u128>(a) * b % modulus)
    );
    assert(math::pow_mod_u64(2, 10, 1000) == 24);
    assert(math::miller_rabin_test(17, 3, 1, 4));
    assert(!math::miller_rabin_test(21, 2, 5, 2));
    assert(math::prime_factorization_internal::pollard_step(
        123'456'789ULL,
        18'446'744'073'709'551'555ULL,
        18'446'744'073'709'551'557ULL
    ) == 15'241'578'750'190'519ULL);

    std::vector<u64> recursive_factors;
    math::factorize_pollard_rho_rec(60, recursive_factors);
    std::sort(recursive_factors.begin(), recursive_factors.end());
    assert((recursive_factors == std::vector<u64>{2, 2, 3, 5}));
    recursive_factors.clear();
    math::factorize_pollard_rho_rec(1, recursive_factors);
    assert(recursive_factors.empty());

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
    assert((math::factorize_pollard_rho(18'446'743'979'220'271'189ULL) ==
        std::vector<std::pair<u64, int>>{
            {4'294'967'279ULL, 1}, {4'294'967'291ULL, 1}
        }));
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

int main(){
    int q;
    if(std::cin >> q){
        while(q--){
            std::string type;
            u64 n;
            std::cin >> type >> n;
            if(type == "STEP"){
                u64 constant, modulus;
                std::cin >> constant >> modulus;
                std::cout << math::prime_factorization_internal::pollard_step(
                    n, constant, modulus) << '\n';
            }else if(type == "PRIME"){
                std::cout << math::is_prime_miller_rabin(n) << '\n';
            }else if(type == "PHI"){
                std::cout << math::euler_phi(n) << '\n';
            }else{
                const auto factors = math::factorize_pollard_rho(n);
                std::cout << factors.size();
                for(auto [prime, exponent]: factors){
                    std::cout << ' ' << prime << ' ' << exponent;
                }
                std::cout << '\n';
            }
        }
        return 0;
    }

    test_miller_rabin();
    test_factorize();
    test_phi();
    test_exceptions();
}
