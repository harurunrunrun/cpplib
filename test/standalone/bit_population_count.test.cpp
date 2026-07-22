// competitive-verifier: STANDALONE

#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/math/combinatorics/bit_population_count.hpp"

static std::string to_decimal(math::WideBitCount value){
    if(value == 0) return "0";
    std::string result;
    while(value != 0){
        result.push_back(static_cast<char>('0' + value % 10));
        value /= 10;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

static void test_bruteforce(){
    std::array<math::WideBitCount, 64> counts{};
    math::WideBitCount total = 0;
    for(std::uint64_t maximum = 0; maximum < 10000; ++maximum){
        for(unsigned bit = 0; bit < 64; ++bit){
            counts[bit] += (maximum >> bit) & 1ULL;
            assert(math::count_bit_ones_inclusive(maximum, bit) == counts[bit]);
        }
        total += std::popcount(maximum);
        assert(math::bit_one_counts_inclusive(maximum) == counts);
        assert(math::total_set_bits_inclusive(maximum) == total);
        assert(math::total_set_bits_half_open(maximum + 1) == total);
    }
}

static void test_boundaries(){
    constexpr std::uint64_t maximum = UINT64_MAX;
    constexpr math::WideBitCount each = math::WideBitCount{1} << 63;
    constexpr auto counts = math::bit_one_counts_inclusive(maximum);
    for(const auto count: counts) assert(count == each);
    assert(math::total_set_bits_inclusive(maximum) == each * 64);
    assert(math::total_set_bits_half_open(maximum) == each * 64 - 64);
    assert(math::count_bit_ones_half_open(0, 0) == 0);
    bool thrown = false;
    try{ (void)math::count_bit_ones_inclusive(0, 64); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

static void test_random_identity(){
    std::mt19937_64 rng(2026071609);
    for(int iteration = 0; iteration < 100000; ++iteration){
        const std::uint64_t value = rng();
        const unsigned bit = static_cast<unsigned>(rng() % 64);
        const math::WideBitCount inclusive =
            math::count_bit_ones_inclusive(value, bit);
        const math::WideBitCount before =
            math::count_bit_ones_half_open(value, bit);
        assert(inclusive - before == ((value >> bit) & 1ULL));
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    if(!(std::cin >> query_count)){
        test_bruteforce();
        test_boundaries();
        test_random_identity();
        return 0;
    }
    while(query_count-- > 0){
        std::string operation;
        std::uint64_t value;
        std::cin >> operation >> value;
        if(operation == "TOTAL_INC"){
            std::cout << to_decimal(math::total_set_bits_inclusive(value)) << '\n';
        }else if(operation == "TOTAL_HALF"){
            std::cout << to_decimal(math::total_set_bits_half_open(value)) << '\n';
        }else{
            unsigned bit;
            std::cin >> bit;
            if(operation == "BIT_INC"){
                std::cout << to_decimal(
                    math::count_bit_ones_inclusive(value, bit)) << '\n';
            }else if(operation == "BIT_HALF"){
                std::cout << to_decimal(
                    math::count_bit_ones_half_open(value, bit)) << '\n';
            }else{
                throw std::runtime_error("unknown bit count operation");
            }
        }
    }
}

