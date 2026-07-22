#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_WILDCARD_PATTERN_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_WILDCARD_PATTERN_MATCHING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "../../../structure/modint/modint.hpp"

namespace wildcard_pattern_matching_detail{

constexpr int first_mod = 998244353;
constexpr int second_mod = 469762049;
constexpr std::size_t maximum_transform_size = std::size_t{1} << 23;
constexpr std::uint64_t maximum_byte_term =
    std::uint64_t{256} * 256 * 255 * 255;

inline std::uint32_t code(char value, char wildcard){
    if(value == wildcard) return 0;
    return static_cast<std::uint32_t>(
        static_cast<unsigned char>(value)
    ) + 1;
}

template<int MOD>
void transform(std::vector<Modint<MOD>>& values, bool inverse = false){
    const int size = static_cast<int>(values.size());
    for(int index = 1, reversed = 0; index < size; ++index){
        int bit = size >> 1;
        while((reversed & bit) != 0){
            reversed ^= bit;
            bit >>= 1;
        }
        reversed ^= bit;
        if(index < reversed) std::swap(values[index], values[reversed]);
    }

    const Modint<MOD> primitive_root = 3;
    for(int length = 2; length <= size; length <<= 1){
        Modint<MOD> root = primitive_root.pow((MOD - 1) / length);
        if(inverse) root = root.inv();
        const int half = length >> 1;
        for(int first = 0; first < size; first += length){
            Modint<MOD> power = 1;
            for(int offset = 0; offset < half; ++offset){
                const Modint<MOD> left = values[first + offset];
                const Modint<MOD> right =
                    values[first + half + offset] * power;
                values[first + offset] = left + right;
                values[first + half + offset] = left - right;
                power *= root;
            }
        }
    }
    if(inverse){
        const Modint<MOD> inverse_size = Modint<MOD>(size).inv();
        for(auto& value: values) value *= inverse_size;
    }
}

template<int MOD>
bool filter_matches(
    std::string_view text,
    std::string_view pattern,
    char wildcard,
    std::size_t transform_size,
    std::vector<bool>& matches
){
    using mint = Modint<MOD>;
    std::vector<mint> text_first(transform_size);
    std::vector<mint> text_second(transform_size);
    std::vector<mint> text_third(transform_size);
    std::vector<mint> pattern_first(transform_size);
    std::vector<mint> pattern_second(transform_size);
    std::vector<mint> pattern_third(transform_size);

    for(std::size_t index = 0; index < text.size(); ++index){
        const mint value = code(text[index], wildcard);
        const mint square = value * value;
        text_first[index] = value;
        text_second[index] = square;
        text_third[index] = square * value;
    }
    for(std::size_t index = 0; index < pattern.size(); ++index){
        const mint value = code(pattern[index], wildcard);
        const mint square = value * value;
        const std::size_t reversed = pattern.size() - 1 - index;
        pattern_first[reversed] = value;
        pattern_second[reversed] = square;
        pattern_third[reversed] = square * value;
    }

    transform(text_first);
    transform(text_second);
    transform(text_third);
    transform(pattern_first);
    transform(pattern_second);
    transform(pattern_third);
    for(std::size_t index = 0; index < transform_size; ++index){
        text_first[index] =
            text_first[index] * pattern_third[index]
            - mint(2) * text_second[index] * pattern_second[index]
            + text_third[index] * pattern_first[index];
    }
    transform(text_first, true);

    bool has_candidate = false;
    const std::size_t offset = pattern.size() - 1;
    for(std::size_t position = 0; position < matches.size(); ++position){
        if(matches[position] && text_first[offset + position] != mint(0)){
            matches[position] = false;
        }
        has_candidate = has_candidate || matches[position];
    }
    return has_candidate;
}

}  // namespace wildcard_pattern_matching_detail

inline std::vector<bool> wildcard_pattern_matching(
    std::string_view text,
    std::string_view pattern,
    char wildcard = '*'
){
    if(pattern.size() > text.size()) return {};
    if(pattern.empty()) return std::vector<bool>(text.size() + 1, true);

    std::vector<bool> matches(text.size() - pattern.size() + 1, true);
    if(std::all_of(pattern.begin(), pattern.end(), [wildcard](char value){
        return value == wildcard;
    }) || std::all_of(text.begin(), text.end(), [wildcard](char value){
        return value == wildcard;
    })){
        return matches;
    }

    const std::size_t convolution_size = text.size() + pattern.size() - 1;
    std::size_t transform_size = 1;
    while(transform_size < convolution_size){
        if(transform_size >= wildcard_pattern_matching_detail::maximum_transform_size){
            throw std::length_error(
                "library assertion fault: transform size is too large "
                "(wildcard_pattern_matching)."
            );
        }
        transform_size <<= 1;
    }
    const std::uint64_t score_bound =
        wildcard_pattern_matching_detail::maximum_byte_term
        * static_cast<std::uint64_t>(pattern.size());
    const std::uint64_t modulus_product =
        std::uint64_t{wildcard_pattern_matching_detail::first_mod}
        * wildcard_pattern_matching_detail::second_mod;
    if(score_bound >= modulus_product)[[unlikely]]{
        throw std::length_error(
            "library assertion fault: exactness bound is exceeded "
            "(wildcard_pattern_matching)."
        );
    }

    const bool has_candidate =
        wildcard_pattern_matching_detail::filter_matches<
            wildcard_pattern_matching_detail::first_mod
        >(text, pattern, wildcard, transform_size, matches);
    if(has_candidate){
        wildcard_pattern_matching_detail::filter_matches<
            wildcard_pattern_matching_detail::second_mod
        >(text, pattern, wildcard, transform_size, matches);
    }
    return matches;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_WILDCARD_PATTERN_MATCHING_HPP_INCLUDED
