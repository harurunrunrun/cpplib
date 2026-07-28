#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_SHIFT_OR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_SHIFT_OR_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

inline std::vector<int> shift_or_match(
    std::string_view text,
    std::string_view pattern
){
    if(pattern.empty()){
        std::vector<int> result(text.size() + 1);
        for(std::size_t index = 0; index <= text.size(); ++index){
            result[index] = static_cast<int>(index);
        }
        return result;
    }
    const std::size_t word_count = (pattern.size() + 63) / 64;
    std::array<std::vector<std::uint64_t>, 256> mask;
    for(auto& row: mask){
        row.assign(word_count, ~std::uint64_t{0});
    }
    for(std::size_t index = 0; index < pattern.size(); ++index){
        mask[static_cast<unsigned char>(pattern[index])][index / 64]
            &= ~(std::uint64_t{1} << (index % 64));
    }
    std::vector<std::uint64_t> state(
        word_count, ~std::uint64_t{0}
    );
    std::vector<int> result;
    for(std::size_t text_index = 0; text_index < text.size(); ++text_index){
        std::uint64_t carry = 0;
        const auto& symbol_mask =
            mask[static_cast<unsigned char>(text[text_index])];
        for(std::size_t word = 0; word < word_count; ++word){
            const std::uint64_t next_carry = state[word] >> 63;
            state[word] = (state[word] << 1 | carry) | symbol_mask[word];
            carry = next_carry;
        }
        if((state[(pattern.size() - 1) / 64]
            >> ((pattern.size() - 1) % 64) & 1U) == 0){
            result.push_back(
                static_cast<int>(text_index + 1 - pattern.size())
            );
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_SHIFT_OR_HPP_INCLUDED
