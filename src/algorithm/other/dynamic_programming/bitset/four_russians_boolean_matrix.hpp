#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_BITSET_FOUR_RUSSIANS_BOOLEAN_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_BITSET_FOUR_RUSSIANS_BOOLEAN_MATRIX_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
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

    for(std::size_t block_begin = 0;
        block_begin < shared;
        block_begin += block_size){
        const std::size_t length =
            std::min(block_size, shared - block_begin);
        const std::size_t pattern_count = std::size_t{1} << length;
        BooleanMatrix table(
            pattern_count,
            std::vector<unsigned char>(column_count)
        );
        for(std::size_t pattern = 1;
            pattern < pattern_count;
            ++pattern){
            const std::size_t bit_index =
                std::countr_zero(pattern);
            const std::size_t previous =
                pattern & (pattern - 1);
            table[pattern] = table[previous];
            const auto& source = second[block_begin + bit_index];
            for(std::size_t column = 0;
                column < column_count;
                ++column){
                table[pattern][column] |= source[column];
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
            for(std::size_t column = 0;
                column < column_count;
                ++column){
                result[row][column] |= table[pattern][column];
            }
        }
    }
    return result;
}

}  // namespace dynamic_programming

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_BITSET_FOUR_RUSSIANS_BOOLEAN_MATRIX_HPP_INCLUDED
