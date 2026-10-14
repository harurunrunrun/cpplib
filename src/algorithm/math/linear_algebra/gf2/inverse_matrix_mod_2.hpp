#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_GF2_INVERSE_MATRIX_MOD_2_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_GF2_INVERSE_MATRIX_MOD_2_HPP_INCLUDED

#include <bitset>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../elimination/gaussian_elimination.hpp"

template<std::size_t MAX_SIZE>
std::optional<std::vector<std::bitset<MAX_SIZE>>> inverse_matrix_mod_2(
    std::vector<std::bitset<MAX_SIZE>> matrix,
    std::size_t size
){
    if(size > MAX_SIZE || matrix.size() != size)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: shape violation (inverse_matrix_mod_2)."
        );
    }

    std::vector<std::bitset<MAX_SIZE>> inverse(size);
    for(std::size_t row = 0; row < size; ++row) inverse[row].set(row);

    for(std::size_t column = 0; column < size; ++column){
        std::size_t pivot = column;
        while(pivot < size && !matrix[pivot].test(column)) ++pivot;
        if(pivot == size) return std::nullopt;
        if(pivot != column){
            std::swap(matrix[pivot], matrix[column]);
            std::swap(inverse[pivot], inverse[column]);
        }
        for(std::size_t row = column + 1; row < size; ++row){
            if(matrix[row].test(column)){
                matrix[row] ^= matrix[column];
                inverse[row] ^= inverse[column];
            }
        }
    }

    for(std::size_t column = size; column-- > 0;){
        for(std::size_t row = 0; row < column; ++row){
            if(matrix[row].test(column)){
                matrix[row] ^= matrix[column];
                inverse[row] ^= inverse[column];
            }
        }
    }
    return inverse;
}

inline std::optional<std::vector<std::vector<bool>>> inverse_matrix_mod_2(
    const std::vector<std::vector<bool>>& input, std::size_t size
){
    if(input.size() != size) throw std::invalid_argument("GF(2) inverse shape");
    auto matrix = gf2_dynamic_internal::pack(input, size);
    for(std::size_t row = 0; row < size; ++row){
        matrix[row].resize((size * 2 + 63) / 64);
        matrix[row][(size + row) / 64] |= std::uint64_t(1) << ((size + row) % 64);
    }
    if(gf2_dynamic_internal::eliminate(matrix, size, true).size() != size) return std::nullopt;
    std::vector<std::vector<bool>> result(size, std::vector<bool>(size));
    for(std::size_t row = 0; row < size; ++row)
        for(std::size_t column = 0; column < size; ++column)
            result[row][column] = gf2_dynamic_internal::bit(matrix[row], size + column);
    return result;
}
inline std::optional<std::vector<std::vector<bool>>> inverse_matrix_mod_2(
    const std::vector<std::vector<bool>>& input
){ return inverse_matrix_mod_2(input, input.size()); }

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_GF2_INVERSE_MATRIX_MOD_2_HPP_INCLUDED
