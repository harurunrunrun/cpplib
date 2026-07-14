#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "../fft/convolution.hpp"

inline int minimum_binary_substring_mismatches(
    const std::string& text,
    const std::string& pattern
){
    const auto is_binary = [](const std::string& value){
        return std::all_of(value.begin(), value.end(), [](char character){
            return character == '0' || character == '1';
        });
    };
    if(!is_binary(text) || !is_binary(pattern))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: non-binary character "
            "(minimum_binary_substring_mismatches)."
        );
    }
    if(pattern.size() > text.size())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: pattern is longer than text "
            "(minimum_binary_substring_mismatches)."
        );
    }
    if(pattern.empty()) return 0;

    constexpr std::size_t MAX_TRANSFORM_SIZE = std::size_t{1} << 23;
    const std::size_t convolution_size = text.size() + pattern.size() - 1;
    if(convolution_size > MAX_TRANSFORM_SIZE)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: transform size is too large "
            "(minimum_binary_substring_mismatches)."
        );
    }

    using Mint = Modint<998244353>;
    std::vector<Mint> text_zero(text.size());
    std::vector<Mint> text_one(text.size());
    std::vector<Mint> pattern_zero(pattern.size());
    std::vector<Mint> pattern_one(pattern.size());
    for(std::size_t index = 0; index < text.size(); index++){
        text_zero[index] = text[index] == '0';
        text_one[index] = text[index] == '1';
    }
    for(std::size_t index = 0; index < pattern.size(); index++){
        const char character = pattern[pattern.size() - 1 - index];
        pattern_zero[index] = character == '0';
        pattern_one[index] = character == '1';
    }

    const auto one_zero = convolution_ntt(text_one, pattern_zero);
    const auto zero_one = convolution_ntt(text_zero, pattern_one);
    int answer = static_cast<int>(pattern.size());
    for(std::size_t left = 0; left + pattern.size() <= text.size(); left++){
        const std::size_t coefficient = left + pattern.size() - 1;
        const int mismatch = one_zero[coefficient].val() +
            zero_one[coefficient].val();
        answer = std::min(answer, mismatch);
    }
    return answer;
}
