#pragma once

#include <vector>

#include "basic_number_theory.hpp"

namespace math{

inline std::vector<long long> enumerate_quotient_values(long long number){
    const std::vector<QuotientRange> ranges = quotient_ranges(number);
    std::vector<long long> result;
    result.reserve(ranges.size());
    for(auto iterator = ranges.rbegin(); iterator != ranges.rend(); ++iterator){
        result.push_back(iterator->value);
    }
    return result;
}

} // namespace math
