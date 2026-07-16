// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>

#include "../../src/structure/other/modulo_multiset_counter.hpp"

static std::string to_decimal(ModuloMultisetCounter::WideCount value){
    if(value == 0) return "0";
    const bool negative = value < 0;
    unsigned __int128 magnitude = static_cast<unsigned __int128>(value);
    if(negative) magnitude = 0 - magnitude;
    std::string result;
    while(magnitude != 0){
        result.push_back(static_cast<char>('0' + magnitude % 10));
        magnitude /= 10;
    }
    if(negative) result.push_back('-');
    std::reverse(result.begin(), result.end());
    return result;
}

static void test_small_random(){
    std::mt19937_64 rng(2026071611);
    for(int iteration = 0; iteration < 400; ++iteration){
        const std::uint64_t maximum = rng() % 80;
        ModuloMultisetCounter counter(maximum);
        std::map<std::uint64_t, long long> reference;
        for(std::uint64_t value = 1; value <= maximum; ++value){
            reference[value]++;
        }
        for(int step = 0; step < 1000; ++step){
            if(rng() % 4 == 0){
                const std::uint64_t modulus = 1 + rng() % 50;
                counter.apply_mod(modulus);
                std::map<std::uint64_t, long long> next;
                for(const auto& [value, count]: reference){
                    next[value % modulus] += count;
                }
                reference = std::move(next);
            }else{
                const std::uint64_t value = rng() % 100;
                const long long delta = static_cast<long long>(rng() % 21) - 10;
                counter.add(value, delta);
                reference[value] += delta;
                if(reference[value] == 0) reference.erase(value);
            }
            for(int probe_count = 0; probe_count < 5; ++probe_count){
                const std::uint64_t probe = rng() % 110;
                assert(counter.count_wide(probe) == reference[probe]);
                assert(counter.count(probe) == reference[probe]);
            }
        }
    }
}

static void test_boundary(){
    ModuloMultisetCounter counter(UINT64_MAX);
    assert(counter.count(0) == 0);
    assert(counter.count(1) == 1);
    assert(counter.count(UINT64_MAX) == 1);
    counter.apply_mod(2);
    const auto half = static_cast<ModuloMultisetCounter::WideCount>(1) << 63;
    assert(counter.count_wide(0) == half - 1);
    assert(counter.count_wide(1) == half);
    counter.apply_mod(1);
    assert(counter.count_wide(0) == (half << 1) - 1);
    bool thrown = false;
    try{ (void)counter.count(0); }catch(const std::overflow_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ counter.apply_mod(0); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    ModCounter alias(5);
    alias.apply_mod(3);
    assert(alias.count(0) == 1);
    assert(alias.count(1) == 2);
    assert(alias.count(2) == 2);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::uint64_t initial_maximum;
    std::size_t query_count;
    if(!(std::cin >> initial_maximum >> query_count)){
        test_small_random();
        test_boundary();
        return 0;
    }
    ModuloMultisetCounter counter(initial_maximum);
    while(query_count-- > 0){
        std::string operation;
        std::uint64_t value;
        std::cin >> operation >> value;
        if(operation == "MOD"){
            counter.apply_mod(value);
        }else if(operation == "ADD"){
            long long delta;
            std::cin >> delta;
            counter.add(value, delta);
        }else if(operation == "COUNT"){
            std::cout << to_decimal(counter.count_wide(value)) << '\n';
        }else{
            throw std::runtime_error("unknown ModuloMultisetCounter operation");
        }
    }
}

