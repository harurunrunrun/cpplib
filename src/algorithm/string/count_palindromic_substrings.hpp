#ifndef CPPLIB_SRC_ALGORITHM_STRING_COUNT_PALINDROMIC_SUBSTRINGS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_COUNT_PALINDROMIC_SUBSTRINGS_HPP_INCLUDED

#include <string>
#include <vector>

#include "manacher.hpp"

inline long long count_palindromic_substrings(const std::string& text){
    long long result = 0;
    for(int radius: manacher_odd(text)) result += radius;
    for(int radius: manacher_even(text)) result += radius;
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_COUNT_PALINDROMIC_SUBSTRINGS_HPP_INCLUDED
