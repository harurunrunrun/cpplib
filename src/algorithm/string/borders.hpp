#ifndef CPPLIB_SRC_ALGORITHM_STRING_BORDERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_BORDERS_HPP_INCLUDED

#include <algorithm>
#include <vector>

#include "prefix_function.hpp"

template<class Sequence>
std::vector<int> border_lengths(const Sequence& sequence){
    const std::vector<int> failure = prefix_function(sequence);
    std::vector<int> result;
    if(failure.empty()) return result;

    for(int length = failure.back(); length > 0; length = failure[length - 1]){
        result.push_back(length);
    }
    std::reverse(result.begin(), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_BORDERS_HPP_INCLUDED
