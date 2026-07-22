// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>

#include "../../src/algorithm/math/number_theory/constant_time_gcd.hpp"

static void test_small(){
    for(std::uint32_t maximum = 0; maximum <= 300; ++maximum){
        ConstantTimeGcd gcd(maximum);
        assert(gcd.max_value() == maximum);
        for(std::uint32_t first = 0; first <= maximum; ++first){
            for(std::uint32_t second = 0; second <= maximum; ++second){
                assert(gcd(first, second) == std::gcd(first, second));
            }
        }
    }
}

static void test_random_and_large(){
    constexpr std::uint32_t maximum = 1000000;
    ConstantTimeGcd gcd(maximum);
    std::mt19937_64 random(2026071652);
    for(int iteration = 0; iteration < 2000000; ++iteration){
        const std::uint32_t first =
            static_cast<std::uint32_t>(random() % (maximum + 1ULL));
        const std::uint32_t second =
            static_cast<std::uint32_t>(random() % (maximum + 1ULL));
        assert(gcd.query(first, second) == std::gcd(first, second));
    }
    assert(gcd.query(999983, 2 * 499979) == 1);
    assert(gcd.query(999983, 999983) == 999983);
    bool thrown = false;
    try{
        (void)gcd.query(maximum + 1, 1);
    }catch(const std::out_of_range&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::uint32_t maximum;
    int query_count;
    if(!(std::cin >> maximum >> query_count)){
        test_small();
        test_random_and_large();
        return 0;
    }
    ConstantTimeGcd gcd(maximum);
    while(query_count-- > 0){
        std::uint32_t first, second;
        std::cin >> first >> second;
        std::cout << gcd.query(first, second) << '\n';
    }
}
