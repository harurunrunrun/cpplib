#ifndef CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_COUNT_PALINDROMIC_SUBSTRINGS_OF_LENGTH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_COUNT_PALINDROMIC_SUBSTRINGS_OF_LENGTH_HPP_INCLUDED

#include <string>

#include "manacher.hpp"

inline long long count_palindromic_substrings_of_length(
    const std::string& text,
    int length
){
    const int size = static_cast<int>(text.size());
    if(length <= 0 || size < length) return 0;

    long long result = 0;
    if((length & 1) != 0){
        const int required_radius = length / 2 + 1;
        for(int radius: manacher_odd(text)){
            if(radius >= required_radius) result++;
        }
    }else{
        const int required_radius = length / 2;
        for(int radius: manacher_even(text)){
            if(radius >= required_radius) result++;
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_COUNT_PALINDROMIC_SUBSTRINGS_OF_LENGTH_HPP_INCLUDED
