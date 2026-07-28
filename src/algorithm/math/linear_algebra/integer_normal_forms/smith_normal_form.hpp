#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_INTEGER_NORMAL_FORMS_SMITH_NORMAL_FORM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_INTEGER_NORMAL_FORMS_SMITH_NORMAL_FORM_HPP_INCLUDED

#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

namespace linear_algebra {

template<class Integer>
struct SmithNormalFormResult {
    std::vector<std::vector<Integer>> diagonal;
    std::vector<std::vector<Integer>> left;
    std::vector<std::vector<Integer>> right;
    std::size_t rank = 0;
};

namespace smith_normal_form_internal {

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
void add_column(
    std::vector<std::vector<Integer>>& matrix,
    std::size_t target,
    std::size_t source,
    const Integer& multiplier
) {
    for(auto& row : matrix){
        row[target] += multiplier * row[source];
    }
}

template<class Integer>
void negate_row(
    std::vector<std::vector<Integer>>& matrix,
    std::size_t row
) {
    for(Integer& value : matrix[row]) value = -value;
}

}  // namespace smith_normal_form_internal

template<class Integer>
SmithNormalFormResult<Integer> smith_normal_form(
    std::vector<std::vector<Integer>> matrix
) {
    const std::size_t row_count = matrix.size();
    const std::size_t column_count =
        row_count == 0 ? 0 : matrix.front().size();
    for(const auto& row : matrix){
        if(row.size() != column_count){
            throw std::invalid_argument(
                "smith_normal_form needs a rectangular matrix"
            );
        }
    }
    SmithNormalFormResult<Integer> result;
    result.diagonal = std::move(matrix);
    result.left =
        smith_normal_form_internal::identity_matrix<Integer>(row_count);
    result.right =
        smith_normal_form_internal::identity_matrix<Integer>(column_count);

    const std::size_t limit = std::min(row_count, column_count);
    for(std::size_t pivot_index = 0; pivot_index < limit; ++pivot_index){
        std::size_t selected_row = row_count;
        std::size_t selected_column = column_count;
        for(std::size_t row = pivot_index;
            row < row_count && selected_row == row_count;
            ++row){
            for(std::size_t column = pivot_index;
                column < column_count;
                ++column){
                if(result.diagonal[row][column] != Integer{}){
                    selected_row = row;
                    selected_column = column;
                    break;
                }
            }
        }
        if(selected_row == row_count) break;
        if(selected_row != pivot_index){
            std::swap(
                result.diagonal[selected_row],
                result.diagonal[pivot_index]
            );
            std::swap(result.left[selected_row], result.left[pivot_index]);
        }
        if(selected_column != pivot_index){
            for(auto& row : result.diagonal){
                std::swap(row[selected_column], row[pivot_index]);
            }
            for(auto& row : result.right){
                std::swap(row[selected_column], row[pivot_index]);
            }
        }

        while(true){
            bool reduced = false;
            for(std::size_t row = pivot_index + 1;
                row < row_count;
                ++row){
                if(result.diagonal[row][pivot_index] == Integer{}) continue;
                const Integer quotient =
                    result.diagonal[row][pivot_index]
                    / result.diagonal[pivot_index][pivot_index];
                smith_normal_form_internal::add_row(
                    result.diagonal, row, pivot_index, -quotient
                );
                smith_normal_form_internal::add_row(
                    result.left, row, pivot_index, -quotient
                );
                if(result.diagonal[row][pivot_index] != Integer{}){
                    std::swap(
                        result.diagonal[row],
                        result.diagonal[pivot_index]
                    );
                    std::swap(result.left[row], result.left[pivot_index]);
                }
                reduced = true;
                break;
            }
            if(reduced) continue;

            for(std::size_t column = pivot_index + 1;
                column < column_count;
                ++column){
                if(result.diagonal[pivot_index][column] == Integer{}) continue;
                const Integer quotient =
                    result.diagonal[pivot_index][column]
                    / result.diagonal[pivot_index][pivot_index];
                smith_normal_form_internal::add_column(
                    result.diagonal, column, pivot_index, -quotient
                );
                smith_normal_form_internal::add_column(
                    result.right, column, pivot_index, -quotient
                );
                if(result.diagonal[pivot_index][column] != Integer{}){
                    for(auto& row : result.diagonal){
                        std::swap(row[column], row[pivot_index]);
                    }
                    for(auto& row : result.right){
                        std::swap(row[column], row[pivot_index]);
                    }
                }
                reduced = true;
                break;
            }
            if(reduced) continue;

            std::size_t violating_row = row_count;
            for(std::size_t row = pivot_index + 1;
                row < row_count && violating_row == row_count;
                ++row){
                for(std::size_t column = pivot_index + 1;
                    column < column_count;
                    ++column){
                    if(result.diagonal[row][column]
                           % result.diagonal[pivot_index][pivot_index]
                       != Integer{}){
                        violating_row = row;
                        break;
                    }
                }
            }
            if(violating_row == row_count) break;
            smith_normal_form_internal::add_row(
                result.diagonal, pivot_index, violating_row, Integer{1}
            );
            smith_normal_form_internal::add_row(
                result.left, pivot_index, violating_row, Integer{1}
            );
        }

        if(result.diagonal[pivot_index][pivot_index] < Integer{}){
            smith_normal_form_internal::negate_row(
                result.diagonal, pivot_index
            );
            smith_normal_form_internal::negate_row(result.left, pivot_index);
        }
        ++result.rank;
    }
    return result;
}

}  // namespace linear_algebra

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_INTEGER_NORMAL_FORMS_SMITH_NORMAL_FORM_HPP_INCLUDED
