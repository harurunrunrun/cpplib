#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_INTEGER_NORMAL_FORMS_HERMITE_NORMAL_FORM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_INTEGER_NORMAL_FORMS_HERMITE_NORMAL_FORM_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace linear_algebra {

template<class Integer>
struct HermiteNormalFormResult {
    std::vector<std::vector<Integer>> matrix;
    std::vector<std::vector<Integer>> left;
    std::vector<std::size_t> pivot_columns;
};

namespace hermite_normal_form_internal {

template<class Integer>
std::vector<std::vector<Integer>> identity_matrix(std::size_t size) {
    std::vector<std::vector<Integer>> result(
        size, std::vector<Integer>(size)
    );
    for(std::size_t index = 0; index < size; ++index){
        result[index][index] = Integer{1};
    }
    return result;
}

template<class Integer>
void add_row(
    std::vector<std::vector<Integer>>& matrix,
    std::size_t target,
    std::size_t source,
    const Integer& multiplier
) {
    for(std::size_t column = 0; column < matrix[target].size(); ++column){
        matrix[target][column] += multiplier * matrix[source][column];
    }
}

template<class Integer>
Integer floor_quotient(Integer numerator, const Integer& denominator) {
    Integer quotient = numerator / denominator;
    const Integer remainder = numerator % denominator;
    if(remainder < Integer{}) --quotient;
    return quotient;
}

}  // namespace hermite_normal_form_internal

template<class Integer>
HermiteNormalFormResult<Integer> row_hermite_normal_form(
    std::vector<std::vector<Integer>> matrix
) {
    const std::size_t row_count = matrix.size();
    const std::size_t column_count =
        row_count == 0 ? 0 : matrix.front().size();
    for(const auto& row : matrix){
        if(row.size() != column_count){
            throw std::invalid_argument(
                "row_hermite_normal_form needs a rectangular matrix"
            );
        }
    }

    HermiteNormalFormResult<Integer> result;
    result.matrix = std::move(matrix);
    result.left =
        hermite_normal_form_internal::identity_matrix<Integer>(row_count);
    std::size_t pivot_row = 0;
    for(std::size_t column = 0;
        column < column_count && pivot_row < row_count;
        ++column){
        std::size_t selected = pivot_row;
        while(selected < row_count
              && result.matrix[selected][column] == Integer{}){
            ++selected;
        }
        if(selected == row_count) continue;
        if(selected != pivot_row){
            std::swap(result.matrix[selected], result.matrix[pivot_row]);
            std::swap(result.left[selected], result.left[pivot_row]);
        }

        while(true){
            std::size_t row = pivot_row + 1;
            while(row < row_count
                  && result.matrix[row][column] == Integer{}){
                ++row;
            }
            if(row == row_count) break;
            const Integer quotient =
                result.matrix[row][column]
                / result.matrix[pivot_row][column];
            hermite_normal_form_internal::add_row(
                result.matrix, row, pivot_row, -quotient
            );
            hermite_normal_form_internal::add_row(
                result.left, row, pivot_row, -quotient
            );
            if(result.matrix[row][column] != Integer{}){
                std::swap(result.matrix[row], result.matrix[pivot_row]);
                std::swap(result.left[row], result.left[pivot_row]);
            }
        }
        if(result.matrix[pivot_row][column] < Integer{}){
            for(Integer& value : result.matrix[pivot_row]) value = -value;
            for(Integer& value : result.left[pivot_row]) value = -value;
        }
        const Integer pivot = result.matrix[pivot_row][column];
        for(std::size_t row = 0; row < pivot_row; ++row){
            const Integer quotient =
                hermite_normal_form_internal::floor_quotient(
                    result.matrix[row][column], pivot
                );
            hermite_normal_form_internal::add_row(
                result.matrix, row, pivot_row, -quotient
            );
            hermite_normal_form_internal::add_row(
                result.left, row, pivot_row, -quotient
            );
        }
        result.pivot_columns.push_back(column);
        ++pivot_row;
    }
    return result;
}

}  // namespace linear_algebra

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_INTEGER_NORMAL_FORMS_HERMITE_NORMAL_FORM_HPP_INCLUDED
