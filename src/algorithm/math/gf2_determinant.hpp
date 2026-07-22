#ifndef CPPLIB_SRC_ALGORITHM_MATH_GF2_DETERMINANT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_GF2_DETERMINANT_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

inline int gf2_determinant(const std::vector<std::string>& matrix){
    const std::size_t size = matrix.size();
    const std::size_t word_count = (size >> 6) + (size & 63U ? 1U : 0U);
    std::vector<std::vector<std::uint64_t>> rows(
        size, std::vector<std::uint64_t>(word_count)
    );
    for(std::size_t row = 0; row < size; ++row){
        if(matrix[row].size() != size)[[unlikely]]{
            throw std::invalid_argument("gf2_determinant requires a square matrix");
        }
        for(std::size_t column = 0; column < size; ++column){
            const char value = matrix[row][column];
            if(value != '0' && value != '1')[[unlikely]]{
                throw std::invalid_argument(
                    "gf2_determinant accepts only binary characters"
                );
            }
            if(value == '1'){
                rows[row][column >> 6] |= std::uint64_t(1) << (column & 63);
            }
        }
    }

    for(std::size_t column = 0; column < size; ++column){
        std::size_t pivot = column;
        while(pivot < size &&
              ((rows[pivot][column >> 6] >> (column & 63)) & 1U) == 0){
            ++pivot;
        }
        if(pivot == size) return 0;
        if(pivot != column) std::swap(rows[pivot], rows[column]);
        for(std::size_t row = column + 1; row < size; ++row){
            if(((rows[row][column >> 6] >> (column & 63)) & 1U) == 0){
                continue;
            }
            for(std::size_t word = column >> 6; word < word_count; ++word){
                rows[row][word] ^= rows[column][word];
            }
        }
    }
    return 1;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_GF2_DETERMINANT_HPP_INCLUDED
