// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "../../src/algorithm/math/number_theory/kth_root_integer.hpp"

namespace{

bool oracle_power_less_equal(
    std::uint64_t base,
    unsigned int exponent,
    std::uint64_t value
){
    std::uint64_t power = 1;
    for(unsigned int index = 0; index < exponent; ++index){
        if(base != 0 && power > value / base) return false;
        power *= base;
    }
    return true;
}

std::uint64_t oracle(std::uint64_t value, unsigned int exponent){
    std::uint64_t lower = 0;
    std::uint64_t upper = value == std::numeric_limits<std::uint64_t>::max()
        ? value : value + 1;
    if(exponent == 1) return value;
    while(lower + 1 < upper){
        const std::uint64_t middle = lower + (upper - lower) / 2;
        if(oracle_power_less_equal(middle, exponent, value)) lower = middle;
        else upper = middle;
    }
    return lower;
}

void self_test(){
    for(std::uint64_t value = 0; value <= 2000; ++value){
        for(unsigned int exponent = 1; exponent <= 64; ++exponent){
            assert(math::kth_root_integer(value, exponent) == oracle(value, exponent));
        }
    }
    constexpr std::uint64_t maximum = std::numeric_limits<std::uint64_t>::max();
    for(unsigned int exponent = 1; exponent <= 64; ++exponent){
        assert(math::kth_root_integer(maximum, exponent) == oracle(maximum, exponent));
    }

    bool thrown = false;
    try{
        (void)math::kth_root_integer(1, 0);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    self_test();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- != 0){
        std::uint64_t value;
        unsigned int exponent;
        std::cin >> value >> exponent;
        std::cout << math::kth_root_integer(value, exponent) << '\n';
    }
}
