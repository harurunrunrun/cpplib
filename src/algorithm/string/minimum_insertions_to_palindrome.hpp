#pragma once

#include <algorithm>

#include "longest_common_subsequence.hpp"

template<class Sequence>
int minimum_insertions_to_palindrome(const Sequence& sequence){
    Sequence reversed = sequence;
    std::reverse(reversed.begin(), reversed.end());
    return static_cast<int>(sequence.size())
        - longest_common_subsequence_length(sequence, reversed);
}
