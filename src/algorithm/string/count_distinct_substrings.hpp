#pragma once

#include "suffix_array.hpp"

template<class Sequence>
long long count_distinct_substrings(const Sequence& sequence){
    const std::vector<int> sa = suffix_array(sequence);
    const std::vector<int> lcp = lcp_array(sequence, sa);
    const long long size = static_cast<long long>(sequence.size());
    long long result = size * (size + 1) / 2;
    for(int common_prefix_length: lcp){
        result -= common_prefix_length;
    }
    return result;
}
