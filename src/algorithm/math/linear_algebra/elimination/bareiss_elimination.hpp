#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_ELIMINATION_BAREISS_ELIMINATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_ELIMINATION_BAREISS_ELIMINATION_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace linear_algebra {

template<class Integer>
struct BareissEliminationResult {
    std::vector<std::vector<Integer>> upper;
    std::size_t rank = 0;
    int row_swap_sign = 1;
};

template<class Integer>
BareissEliminationResult<Integer> bareiss_elimination(
    std::vector<std::vector<Integer>> matrix
) {
    const std::size_t row_count = matrix.size();
    const std::size_t column_count =
        row_count == 0 ? 0 : matrix.front().size();
    for(const auto& row : matrix){
        if(row.size() != column_count){
            throw std::invalid_argument(
                "bareiss_elimination needs a rectangular matrix"
            );
        }
    }

    BareissEliminationResult<Integer> result;
    result.upper = std::move(matrix);
    Integer previous_pivot = Integer{1};
    std::size_t pivot_row = 0;
    for(std::size_t column = 0;
        column < column_count && pivot_row < row_count;
        ++column){
        std::size_t selected = pivot_row;
        while(selected < row_count
              && result.upper[selected][column] == Integer{}){
            ++selected;
        }
        if(selected == row_count) continue;
        if(selected != pivot_row){
            std::swap(result.upper[selected], result.upper[pivot_row]);
            result.row_swap_sign = -result.row_swap_sign;
        }
        const Integer pivot = result.upper[pivot_row][column];
        for(std::size_t row = pivot_row + 1; row < row_count; ++row){
            for(std::size_t next_column = column + 1;
                next_column < column_count;
                ++next_column){
                Integer numerator =
                    result.upper[row][next_column] * pivot
                    - result.upper[row][column]
                        * result.upper[pivot_row][next_column];
                if(pivot_row != 0) numerator /= previous_pivot;
                result.upper[row][next_column] = std::move(numerator);
            }
            result.upper[row][column] = Integer{};
        }
        previous_pivot = pivot;
        ++pivot_row;
    }
    result.rank = pivot_row;
    return result;
}

template<class Integer>
Integer bareiss_determinant(std::vector<std::vector<Integer>> matrix) {
    const std::size_t size = matrix.size();
    for(const auto& row : matrix){
        if(row.size() != size){
            throw std::invalid_argument(
                "bareiss_determinant needs a square matrix"
            );
        }
    }
    if(size == 0) return Integer{1};
    const auto result = bareiss_elimination(std::move(matrix));
    if(result.rank != size) return Integer{};
    Integer determinant = result.upper.back().back();
    if(result.row_swap_sign < 0) determinant = -determinant;
    return determinant;
}

}  // namespace linear_algebra

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_ELIMINATION_BAREISS_ELIMINATION_HPP_INCLUDED
