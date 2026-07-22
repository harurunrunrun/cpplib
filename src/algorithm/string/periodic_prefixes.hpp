#ifndef CPPLIB_SRC_ALGORITHM_STRING_PERIODIC_PREFIXES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_PERIODIC_PREFIXES_HPP_INCLUDED

#include <utility>
#include <vector>

#include "prefix_function.hpp"

template<class Sequence>
std::vector<std::pair<int, int>> periodic_prefixes(const Sequence& sequence){
    const int size = static_cast<int>(sequence.size());
    const std::vector<int> failure = prefix_function(sequence);
    std::vector<std::pair<int, int>> result;
    for(int prefix_length = 2; prefix_length <= size; prefix_length++){
        const int block_length = prefix_length - failure[prefix_length - 1];
        if(prefix_length % block_length != 0) continue;
        const int repetition_count = prefix_length / block_length;
        if(repetition_count > 1){
            result.emplace_back(prefix_length, repetition_count);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_PERIODIC_PREFIXES_HPP_INCLUDED
