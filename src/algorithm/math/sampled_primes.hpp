#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace math{

// Returns primes whose one-based prime index is congruent to 1 modulo stride.
inline std::vector<int> sampled_primes(int exclusive_limit, int stride = 100){
    if(exclusive_limit < 0 || stride <= 0)[[unlikely]]{
        throw std::invalid_argument("invalid sampled_primes arguments");
    }
    if(exclusive_limit <= 2) return {};
    const int odd_count = exclusive_limit / 2;
    std::vector<bool> composite(static_cast<std::size_t>(odd_count));
    for(int value = 3; value <= (exclusive_limit - 1) / value; value += 2){
        if(composite[static_cast<std::size_t>(value / 2)]) continue;
        for(int multiple = value * value; multiple < exclusive_limit;
            multiple += 2 * value
        ){
            composite[static_cast<std::size_t>(multiple / 2)] = true;
        }
    }
    std::vector<int> result;
    int prime_index = 1;
    result.push_back(2);
    for(int value = 3; value < exclusive_limit; value += 2){
        if(composite[static_cast<std::size_t>(value / 2)]) continue;
        ++prime_index;
        if((prime_index - 1) % stride == 0) result.push_back(value);
    }
    return result;
}

} // namespace math
