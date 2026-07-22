#ifndef CPPLIB_SRC_ALGORITHM_STRING_ENUMERATE_PALINDROME_LENGTHS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_ENUMERATE_PALINDROME_LENGTHS_HPP_INCLUDED

#include <string>
#include <vector>

#include "manacher.hpp"

inline std::vector<int> enumerate_palindrome_lengths(const std::string& text){
    if(text.empty()) return {};
    const std::vector<int> odd = manacher_odd(text);
    const std::vector<int> even = manacher_even(text);
    std::vector<int> result(text.size() * 2 - 1);
    for(std::size_t center = 0; center < result.size(); ++center){
        if((center & 1U) == 0){
            result[center] = 2 * odd[center / 2] - 1;
        }else{
            result[center] = 2 * even[center / 2 + 1];
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_ENUMERATE_PALINDROME_LENGTHS_HPP_INCLUDED
