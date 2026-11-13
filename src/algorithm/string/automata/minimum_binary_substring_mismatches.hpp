#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_MINIMUM_BINARY_SUBSTRING_MISMATCHES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_MINIMUM_BINARY_SUBSTRING_MISMATCHES_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../fft/transform/convolution.hpp"

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

    // Map '0' -> -1 and '1' -> +1; the correlation at an alignment equals
    // matches - mismatches = |pattern| - 2 * mismatches.
    constexpr long long MOD = 998244353;
    using Mint = Modint<static_cast<int>(MOD)>;
    std::vector<Mint> text_sign(text.size());
    std::vector<Mint> pattern_sign(pattern.size());
    for(std::size_t index = 0; index < text.size(); index++){
        text_sign[index] = text[index] == '1' ? 1 : MOD - 1;
    }
    for(std::size_t index = 0; index < pattern.size(); index++){
        const char character = pattern[pattern.size() - 1 - index];
        pattern_sign[index] = character == '1' ? 1 : MOD - 1;
    }

    const auto correlation = convolution_ntt(text_sign, pattern_sign);
    const long long length = static_cast<long long>(pattern.size());
    int answer = static_cast<int>(pattern.size());
    for(std::size_t left = 0; left + pattern.size() <= text.size(); left++){
        const std::size_t coefficient = left + pattern.size() - 1;
        long long signed_value = correlation[coefficient].val();
        if(signed_value > MOD / 2){
            signed_value -= MOD;
        }
        const int mismatch = static_cast<int>((length - signed_value) / 2);
        answer = std::min(answer, mismatch);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_MINIMUM_BINARY_SUBSTRING_MISMATCHES_HPP_INCLUDED
