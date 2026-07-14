#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include "manacher.hpp"

inline std::string shortest_palindrome_by_appending(std::string_view text){
    std::string result(text);
    const int size = static_cast<int>(result.size());
    int longest_palindromic_suffix = 0;

    {
        const std::vector<int> odd_radius = manacher_odd(result);
        for(int center = 0; center < size; center++){
            if(center + odd_radius[center] == size){
                longest_palindromic_suffix = std::max(
                    longest_palindromic_suffix,
                    2 * odd_radius[center] - 1
                );
            }
        }
    }
    {
        const std::vector<int> even_radius = manacher_even(result);
        for(int center = 0; center < size; center++){
            if(center + even_radius[center] == size){
                longest_palindromic_suffix = std::max(
                    longest_palindromic_suffix,
                    2 * even_radius[center]
                );
            }
        }
    }

    result.reserve(
        result.size()
        + result.size()
        - static_cast<std::size_t>(longest_palindromic_suffix)
    );
    for(int index = size - longest_palindromic_suffix - 1; index >= 0; index--){
        result.push_back(result[static_cast<std::size_t>(index)]);
    }
    return result;
}
