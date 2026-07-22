#ifndef CPPLIB_SRC_ALGORITHM_MATH_PFAFFIAN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_PFAFFIAN_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../structure/matrix/matrix.hpp"

namespace pfaffian_detail{

template<class T>
void validate_alternating_matrix(const std::vector<std::vector<T>>& matrix){
    const std::size_t size = matrix.size();
    if(size % 2 != 0)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: odd size (pfaffian)."
        );
    }
    for(const auto& row: matrix){
        if(row.size() != size)[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: square violation (pfaffian)."
            );
        }
    }
    for(std::size_t row = 0; row < size; ++row){
        if(matrix[row][row] != T{})[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: diagonal violation (pfaffian)."
            );
        }
        for(std::size_t column = row + 1; column < size; ++column){
            if(matrix[column][row] != -matrix[row][column])[[unlikely]]{
                throw std::invalid_argument(
                    "library assertion fault: alternating violation (pfaffian)."
                );
            }
        }
    }
}

}  // namespace pfaffian_detail

template<class T>
T pfaffian(std::vector<std::vector<T>> matrix){
    pfaffian_detail::validate_alternating_matrix(matrix);
    const std::size_t size = matrix.size();
    T result = T(1);
    for(std::size_t first = 0; first < size; first += 2){
        std::size_t pivot = first + 1;
        while(pivot < size && matrix[first][pivot] == T{}) ++pivot;
        if(pivot == size) return T{};
        if(pivot != first + 1){
            std::swap(matrix[pivot], matrix[first + 1]);
            for(std::size_t row = 0; row < size; ++row){
                std::swap(matrix[row][pivot], matrix[row][first + 1]);
            }
            result = -result;
        }

        const T pivot_value = matrix[first][first + 1];
        result *= pivot_value;
        const T inverse = T(1) / pivot_value;
        for(std::size_t row = first + 2; row < size; ++row){
            for(std::size_t column = row + 1; column < size; ++column){
                matrix[row][column] -= (
                    matrix[first][row] * matrix[first + 1][column]
                    - matrix[first][column] * matrix[first + 1][row]
                ) * inverse;
                matrix[column][row] = -matrix[row][column];
            }
        }
    }
    return result;
}

template<class T, int MAX_ROW, int MAX_COL>
T pfaffian(const Matrix<T, MAX_ROW, MAX_COL>& matrix){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: square violation (pfaffian)."
        );
    }
    std::vector<std::vector<T>> values(
        static_cast<std::size_t>(matrix.rows()),
        std::vector<T>(static_cast<std::size_t>(matrix.cols()))
    );
    for(int row = 0; row < matrix.rows(); ++row){
        for(int column = 0; column < matrix.cols(); ++column){
            values[static_cast<std::size_t>(row)]
                [static_cast<std::size_t>(column)] = matrix(row, column);
        }
    }
    return pfaffian(std::move(values));
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_PFAFFIAN_HPP_INCLUDED
