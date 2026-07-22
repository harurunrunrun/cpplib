// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/math/integer/big_integer_gcd.hpp"

static void test_edges(){
    assert(big_integer_gcd(0, 0) == 0);
    assert(big_integer_gcd(-42, 0) == 42);
    assert(big_integer_gcd(0, -42) == 42);
    assert(big_integer_lcm(0, 99) == 0);
    assert(big_integer_lcm(-12, 18) == 36);
    const BigInteger common("123456789012345678901234567890");
    const BigInteger first = common * BigInteger("9876543210987654321");
    const BigInteger second = common * BigInteger("1111111111111111111");
    assert(big_integer_gcd(first, second) == common);
}

static void test_extended_random(){
    std::mt19937_64 rng(2026071617);
    for(int iteration = 0; iteration < 100000; ++iteration){
        const std::int64_t first = static_cast<std::int64_t>(rng() % 2'000'000'001ULL)
            - 1'000'000'000LL;
        const std::int64_t second = static_cast<std::int64_t>(rng() % 2'000'000'001ULL)
            - 1'000'000'000LL;
        const auto result = big_integer_extended_gcd(first, second);
        const std::uint64_t expected = std::gcd(first, second);
        assert(result.gcd == BigInteger(expected));
        assert(BigInteger(first) * result.x + BigInteger(second) * result.y
            == result.gcd);
        assert(big_integer_gcd(first, second) == result.gcd);
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    if(!(std::cin >> query_count)){
        test_edges();
        test_extended_random();
        return 0;
    }
    while(query_count-- > 0){
        std::string operation, first, second;
        std::cin >> operation >> first >> second;
        if(operation == "GCD"){
            std::cout << big_integer_gcd(BigInteger(first), BigInteger(second)) << '\n';
        }else if(operation == "LCM"){
            std::cout << big_integer_lcm(BigInteger(first), BigInteger(second)) << '\n';
        }else if(operation == "EXT"){
            const BigInteger a(first), b(second);
            const auto result = big_integer_extended_gcd(a, b);
            if(a * result.x + b * result.y != result.gcd){
                throw std::runtime_error("invalid extended gcd identity");
            }
            std::cout << result.gcd << '\n';
        }else{
            throw std::runtime_error("unknown BigInteger GCD operation");
        }
    }
}

