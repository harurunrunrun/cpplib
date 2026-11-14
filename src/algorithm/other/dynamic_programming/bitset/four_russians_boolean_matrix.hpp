#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_BITSET_FOUR_RUSSIANS_BOOLEAN_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_BITSET_FOUR_RUSSIANS_BOOLEAN_MATRIX_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace dynamic_programming {

using BooleanMatrix = std::vector<std::vector<unsigned char>>;

inline BooleanMatrix four_russians_boolean_matrix_multiply(
    const BooleanMatrix& first,
    const BooleanMatrix& second,
    std::size_t block_size = 0
) {
    const std::size_t row_count = first.size();
    const std::size_t shared =
        row_count == 0 ? second.size() : first.front().size();
    for(const auto& row : first){
        if(row.size() != shared){
            throw std::invalid_argument(
                "first Boolean matrix is not rectangular"
            );
        }
        for(const unsigned char value : row){
            if(value > 1){
                throw std::invalid_argument(
                    "Boolean matrix entry must be 0 or 1"
                );
            }
        }
    }
    if(second.size() != shared){
        throw std::invalid_argument(
            "Boolean matrix dimensions do not match"
        );
    }
    const std::size_t column_count =
        second.empty() ? 0 : second.front().size();
    for(const auto& row : second){
        if(row.size() != column_count){
            throw std::invalid_argument(
                "second Boolean matrix is not rectangular"
            );
        }
        for(const unsigned char value : row){
            if(value > 1){
                throw std::invalid_argument(
                    "Boolean matrix entry must be 0 or 1"
                );
            }
        }
    }
    BooleanMatrix result(
        row_count, std::vector<unsigned char>(column_count)
    );
    if(shared == 0 || row_count == 0 || column_count == 0) return result;
    if(block_size == 0){
        block_size = std::max<std::size_t>(
            1, std::bit_width(shared) / 2
        );
    }
    if(block_size >= std::numeric_limits<std::size_t>::digits
       || block_size > 20){
        throw std::invalid_argument(
            "Four Russians block size must be at most 20"
        );
    }

    // Rows are packed into 64-bit words so that table entries and result
    // rows are OR-ed a word at a time.
    constexpr std::size_t word_bits = 64;
    const std::size_t word_count = (column_count + word_bits - 1) / word_bits;
    std::vector<std::uint64_t> packed_second(shared * word_count, 0);
    for(std::size_t index = 0; index < shared; ++index){
        std::uint64_t* const destination =
            packed_second.data() + index * word_count;
        for(std::size_t column = 0; column < column_count; ++column){
            destination[column / word_bits] |=
                static_cast<std::uint64_t>(second[index][column])
                << (column % word_bits);
        }
    }
    std::vector<std::uint64_t> packed_result(row_count * word_count, 0);
    std::vector<std::uint64_t> table;
    for(std::size_t block_begin = 0;
        block_begin < shared;
        block_begin += block_size){
        const std::size_t length =
            std::min(block_size, shared - block_begin);
        const std::size_t pattern_count = std::size_t{1} << length;
        table.assign(pattern_count * word_count, 0);
        for(std::size_t pattern = 1;
            pattern < pattern_count;
            ++pattern){
            const std::size_t bit_index =
                std::countr_zero(pattern);
            const std::size_t previous =
                pattern & (pattern - 1);
            std::uint64_t* const destination =
                table.data() + pattern * word_count;
            const std::uint64_t* const base =
                table.data() + previous * word_count;
            const std::uint64_t* const source = packed_second.data()
                + (block_begin + bit_index) * word_count;
            for(std::size_t word = 0; word < word_count; ++word){
                destination[word] = base[word] | source[word];
            }
        }
        for(std::size_t row = 0; row < row_count; ++row){
            std::size_t pattern = 0;
            for(std::size_t offset = 0; offset < length; ++offset){
                pattern |=
                    static_cast<std::size_t>(
                        first[row][block_begin + offset]
                    ) << offset;
            }
            if(pattern == 0) continue;
            std::uint64_t* const destination =
                packed_result.data() + row * word_count;
            const std::uint64_t* const source =
                table.data() + pattern * word_count;
            for(std::size_t word = 0; word < word_count; ++word){
                destination[word] |= source[word];
            }
        }
    }
    for(std::size_t row = 0; row < row_count; ++row){
        const std::uint64_t* const source =
            packed_result.data() + row * word_count;
        for(std::size_t column = 0; column < column_count; ++column){
            result[row][column] = static_cast<unsigned char>(
                (source[column / word_bits] >> (column % word_bits)) & 1U
            );
        }
    }
    return result;
}

}  // namespace dynamic_programming

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_BITSET_FOUR_RUSSIANS_BOOLEAN_MATRIX_HPP_INCLUDED
