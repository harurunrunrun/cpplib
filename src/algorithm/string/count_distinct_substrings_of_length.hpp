#pragma once

#include "suffix_array.hpp"

template<class Sequence>
long long count_distinct_substrings_of_length(
    const Sequence& sequence,
    int length
){
    const int size = static_cast<int>(sequence.size());
    if(length <= 0 || size < length) return 0;

    const std::vector<int> sa = suffix_array(sequence);
    const std::vector<int> lcp = lcp_array(sequence, sa);
    long long result = 0;
    bool previous_suffix_is_long_enough = false;
    for(int rank = 0; rank < size; rank++){
        const bool long_enough = size - sa[static_cast<std::size_t>(rank)] >= length;
        if(long_enough && (!previous_suffix_is_long_enough
            || lcp[static_cast<std::size_t>(rank - 1)] < length)){
            result++;
        }
        previous_suffix_is_long_enough = long_enough;
    }
    return result;
}
