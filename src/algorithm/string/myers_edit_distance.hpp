#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

inline std::size_t myers_bit_parallel_edit_distance(
    std::string_view first,
    std::string_view second
){
    if(first.size() > second.size()) std::swap(first, second);
    const std::string_view pattern = first;
    const std::string_view text = second;
    if(pattern.empty()) return text.size();

    constexpr std::size_t word_bits = 64;
    const std::size_t block_count =
        (pattern.size() + word_bits - 1) / word_bits;
    const std::size_t final_bits = pattern.size() % word_bits;
    const std::uint64_t final_mask = final_bits == 0
        ? ~std::uint64_t(0)
        : (std::uint64_t(1) << final_bits) - 1;
    const std::uint64_t score_bit =
        std::uint64_t(1) << ((pattern.size() - 1) % word_bits);

    std::array<std::vector<std::uint64_t>, 256> equality;
    for(std::size_t position = 0; position < pattern.size(); position++){
        const auto character = static_cast<unsigned char>(pattern[position]);
        auto& mask = equality[character];
        if(mask.empty()) mask.assign(block_count, 0);
        mask[position / word_bits] |=
            std::uint64_t(1) << (position % word_bits);
    }

    std::vector<std::uint64_t> positive_vertical(
        block_count,
        ~std::uint64_t(0)
    );
    positive_vertical.back() &= final_mask;
    std::vector<std::uint64_t> negative_vertical(block_count);
    std::size_t score = pattern.size();

    for(char raw_character: text){
        const auto character =
            static_cast<unsigned char>(raw_character);
        const auto& equality_mask = equality[character];
        const bool character_occurs = !equality_mask.empty();

        std::uint64_t addition_carry = 0;
        std::uint64_t positive_shift_carry = 1;
        std::uint64_t negative_shift_carry = 0;
        for(std::size_t block = 0; block < block_count; block++){
            const std::uint64_t word_mask =
                block + 1 == block_count
                    ? final_mask
                    : ~std::uint64_t(0);
            const std::uint64_t equal = character_occurs
                ? equality_mask[block]
                : 0;
            const std::uint64_t positive =
                positive_vertical[block];
            const std::uint64_t negative =
                negative_vertical[block];

            const std::uint64_t addend = equal & positive;
            const std::uint64_t partial_sum = addend + positive;
            const bool first_carry = partial_sum < addend;
            const std::uint64_t sum =
                partial_sum + addition_carry;
            const bool second_carry = sum < partial_sum;
            addition_carry = first_carry || second_carry;

            const std::uint64_t horizontal =
                ((sum ^ positive) | equal) & word_mask;
            const std::uint64_t vertical =
                (equal | negative) & word_mask;
            const std::uint64_t positive_horizontal =
                (negative | ~(horizontal | positive)) & word_mask;
            const std::uint64_t negative_horizontal =
                (positive & horizontal) & word_mask;

            if(block + 1 == block_count){
                if(positive_horizontal & score_bit) score++;
                if(negative_horizontal & score_bit) score--;
            }

            const std::uint64_t next_positive_shift_carry =
                positive_horizontal >> (word_bits - 1);
            const std::uint64_t next_negative_shift_carry =
                negative_horizontal >> (word_bits - 1);
            const std::uint64_t shifted_positive =
                (positive_horizontal << 1) |
                positive_shift_carry;
            const std::uint64_t shifted_negative =
                (negative_horizontal << 1) |
                negative_shift_carry;
            positive_shift_carry = next_positive_shift_carry;
            negative_shift_carry = next_negative_shift_carry;

            positive_vertical[block] =
                (shifted_negative |
                    ~(vertical | shifted_positive)) &
                word_mask;
            negative_vertical[block] =
                (shifted_positive & vertical) & word_mask;
        }
    }
    return score;
}
