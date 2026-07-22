#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_INVERSE_MATRIX_MOD_2_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_INVERSE_MATRIX_MOD_2_HPP_INCLUDED

#include <bitset>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

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

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_INVERSE_MATRIX_MOD_2_HPP_INCLUDED
