#ifndef CPPLIB_SRC_ALGORITHM_STRING_MINIMUM_INSERTIONS_TO_PALINDROME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_MINIMUM_INSERTIONS_TO_PALINDROME_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

#include "longest_common_subsequence.hpp"

inline std::size_t minimum_insertions_to_palindrome_bytes(
    std::string_view sequence
){
    std::string reversed(sequence);
    std::reverse(reversed.begin(), reversed.end());
    return sequence.size()
        - longest_common_subsequence_length_bytes(sequence, reversed);
}

inline int minimum_insertions_to_palindrome(std::string_view sequence){
    if(sequence.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error(
            "minimum_insertions_to_palindrome result does not fit in int"
        );
    }
    return static_cast<int>(minimum_insertions_to_palindrome_bytes(sequence));
}

inline int minimum_insertions_to_palindrome(const std::string& sequence){
    return minimum_insertions_to_palindrome(std::string_view(sequence));
}

template<class Sequence>
int minimum_insertions_to_palindrome(const Sequence& sequence){
    Sequence reversed = sequence;
    std::reverse(reversed.begin(), reversed.end());
    return static_cast<int>(sequence.size())
        - longest_common_subsequence_length(sequence, reversed);
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_MINIMUM_INSERTIONS_TO_PALINDROME_HPP_INCLUDED
