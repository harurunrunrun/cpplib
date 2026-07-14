// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <iostream>
#include <set>
#include <vector>

#include "../../src/approximate/streaming/morris_counter.hpp"
#include "../../src/approximate/streaming/reservoir_sampling.hpp"

int main(){
    int count;
    if(!(std::cin >> count)) return 0;
    ReservoirSampler<int, 32> first(42), second(42);
    WeightedReservoirSampler<int, 32> weighted_first(84), weighted_second(84);
    std::set<int> input;
    for(int i = 0; i < count; ++i){
        int value;
        long double weight;
        std::cin >> value >> weight;
        input.insert(value);
        first.add(value);
        second.add(value);
        weighted_first.add(value, weight);
        weighted_second.add(value, weight);
    }
    assert(first.samples() == second.samples());
    assert(weighted_first.samples() == weighted_second.samples());
    assert(first.size() == std::min<std::size_t>(32U, static_cast<std::size_t>(count)));
    assert(first.seen() == static_cast<std::uint64_t>(count));
    assert(weighted_first.seen() == static_cast<std::uint64_t>(count));
    assert((ReservoirSampler<int, 32>::capacity() == 32U));
    assert((WeightedReservoirSampler<int, 32>::capacity() == 32U));
    assert(weighted_first.size() == first.size());
    for(const int value: first.samples()) assert(input.count(value) == 1U);
    for(const int value: weighted_first.samples()) assert(input.count(value) == 1U);

    MorrisCounter morris_first(2026), morris_second(2026);
    for(int i = 0; i < count * 20; ++i){
        morris_first.increment();
        morris_second.increment();
    }
    assert(morris_first.exponent() == morris_second.exponent());
    const long double exact = static_cast<long double>(count * 20);
    assert(morris_first.estimate() >= exact / 8.0L);
    assert(morris_first.estimate() <= exact * 8.0L + 1.0L);

    bool thrown = false;
    try{
        weighted_first.add(0, 0.0L);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        weighted_first.add(0, std::numeric_limits<long double>::infinity());
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        MorrisCounter invalid_morris(0, std::numeric_limits<long double>::infinity());
        (void)invalid_morris;
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    first.clear(777);
    second.clear(777);
    assert(first.size() == 0U);
    assert(first.seen() == 0U);
    assert(first.samples().empty());
    for(int value = 0; value < 100; ++value){
        first.add(value);
        second.add(value);
    }
    assert(first.samples() == second.samples());
    assert(first.seen() == 100U);

    weighted_first.clear(888);
    weighted_second.clear(888);
    assert(weighted_first.size() == 0U);
    assert(weighted_first.seen() == 0U);
    assert(weighted_first.samples().empty());
    for(int value = 0; value < 100; ++value){
        weighted_first.add(value, static_cast<long double>(value + 1));
        weighted_second.add(value, static_cast<long double>(value + 1));
    }
    assert(weighted_first.samples() == weighted_second.samples());
    assert(weighted_first.seen() == 100U);

    assert(morris_first.base() == 2.0L);
    morris_first.clear(999);
    morris_second.clear(999);
    assert(morris_first.exponent() == 0U);
    assert(morris_first.estimate() == 0.0L);
    for(int i = 0; i < 1000; ++i){
        morris_first.increment();
        morris_second.increment();
    }
    assert(morris_first.exponent() == morris_second.exponent());
    std::cout << "OK\n";
}
