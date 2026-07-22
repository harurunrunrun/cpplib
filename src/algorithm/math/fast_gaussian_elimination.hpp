#ifndef CPPLIB_SRC_ALGORITHM_MATH_FAST_GAUSSIAN_ELIMINATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_FAST_GAUSSIAN_ELIMINATION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "fast_characteristic_polynomial.hpp"
#include "fast_determinant.hpp"
#include "gaussian_elimination.hpp"

namespace fast_gaussian_elimination_internal{

inline std::size_t checked_area(std::size_t rows, std::size_t columns){
    if(rows != 0 && columns > std::numeric_limits<std::size_t>::max() / rows){
        throw std::length_error(
            "library assertion fault: size overflow "
            "(fast_gaussian_elimination)."
        );
    }
    return rows * columns;
}

inline void check_solution_column_count(std::size_t columns){
    if(columns > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error(
            "library assertion fault: too many columns "
            "(fast_gaussian_elimination)."
        );
    }
}

template<class T>
void check_flat_shape(
    const std::vector<T>& matrix,
    std::size_t rows,
    std::size_t columns
){
    if(matrix.size() != checked_area(rows, columns))[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: shape violation "
            "(fast_gaussian_elimination)."
        );
    }
}

template<class T>
std::pair<std::size_t, std::size_t> nested_shape(
    const std::vector<std::vector<T>>& matrix
){
    const std::size_t rows = matrix.size();
    const std::size_t columns = rows == 0 ? 0 : matrix.front().size();
    (void)checked_area(rows, columns);
    for(const auto& row: matrix){
        if(row.size() != columns)[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: rectangular violation "
                "(fast_gaussian_elimination)."
            );
        }
    }
    return {rows, columns};
}

template<class T>
std::vector<T> flatten(const std::vector<std::vector<T>>& matrix){
    const auto [rows, columns] = nested_shape(matrix);
    std::vector<T> values(checked_area(rows, columns));
    for(std::size_t row = 0; row < rows; ++row){
        std::copy_n(
            matrix[row].begin(), columns, values.begin() + row * columns
        );
    }
    return values;
}

template<class T, int MAX_ROW, int MAX_COL>
std::vector<T> flatten(const Matrix<T, MAX_ROW, MAX_COL>& matrix){
    const std::size_t rows = static_cast<std::size_t>(matrix.rows());
    const std::size_t columns = static_cast<std::size_t>(matrix.cols());
    std::vector<T> values(checked_area(rows, columns));
    for(std::size_t row = 0; row < rows; ++row){
        for(std::size_t column = 0; column < columns; ++column){
            values[row * columns + column] = matrix(
                static_cast<int>(row), static_cast<int>(column)
            );
        }
    }
    return values;
}

template<class T>
std::vector<std::size_t> column_rank_profile(
    const std::vector<T>& matrix,
    std::size_t rows,
    std::size_t columns
){
    check_flat_shape(matrix, rows, columns);
    std::vector<T> transpose(checked_area(columns, rows));
    for(std::size_t row = 0; row < rows; ++row){
        for(std::size_t column = 0; column < columns; ++column){
            transpose[column * rows + row] = matrix[row * columns + column];
        }
    }
    std::vector<std::size_t> column_indices(columns);
    std::iota(column_indices.begin(), column_indices.end(), std::size_t(0));
    const auto profile =
        fast_characteristic_polynomial_internal::ordered_row_echelon(
            transpose, columns, rows, column_indices
        );
    return profile.pivot_rows;
}

template<class T>
std::vector<T> left_solve_upper(
    const std::vector<T>& upper,
    std::size_t size,
    const std::vector<T>& right_hand_side,
    std::size_t right_columns
){
    if(upper.size() != checked_area(size, size) ||
       right_hand_side.size() != checked_area(size, right_columns))[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: internal shape violation "
            "(fast_gaussian_elimination)."
        );
    }
    if(size == 0 || right_columns == 0) return right_hand_side;
    if(size <= 32){
        std::vector<T> solution(right_hand_side);
        for(std::size_t row = size; row-- > 0;){
            const T diagonal = upper[row * size + row];
            if(diagonal == T{})[[unlikely]]{
                throw std::logic_error(
                    "library assertion fault: zero pivot "
                    "(fast_gaussian_elimination)."
                );
            }
            for(std::size_t column = 0; column < right_columns; ++column){
                T value = solution[row * right_columns + column];
                for(std::size_t next = row + 1; next < size; ++next){
                    value -= upper[row * size + next] *
                        solution[next * right_columns + column];
                }
                solution[row * right_columns + column] = value / diagonal;
            }
        }
        return solution;
    }

    const std::size_t first_size = size / 2;
    const std::size_t second_size = size - first_size;
    std::vector<T> upper_first(checked_area(first_size, first_size));
    std::vector<T> upper_cross(checked_area(first_size, second_size));
    std::vector<T> upper_second(checked_area(second_size, second_size));
    for(std::size_t row = 0; row < first_size; ++row){
        for(std::size_t column = 0; column < first_size; ++column){
            upper_first[row * first_size + column] =
                upper[row * size + column];
        }
        for(std::size_t column = 0; column < second_size; ++column){
            upper_cross[row * second_size + column] =
                upper[row * size + first_size + column];
        }
    }
    for(std::size_t row = 0; row < second_size; ++row){
        for(std::size_t column = 0; column < second_size; ++column){
            upper_second[row * second_size + column] = upper[
                (first_size + row) * size + first_size + column
            ];
        }
    }

    std::vector<T> first_rhs(checked_area(first_size, right_columns));
    std::vector<T> second_rhs(checked_area(second_size, right_columns));
    std::copy_n(
        right_hand_side.begin(), first_rhs.size(), first_rhs.begin()
    );
    std::copy_n(
        right_hand_side.begin() + static_cast<std::ptrdiff_t>(first_rhs.size()),
        second_rhs.size(), second_rhs.begin()
    );
    std::vector<T> second_solution = left_solve_upper(
        upper_second, second_size, second_rhs, right_columns
    );
    const std::vector<T> contribution = fast_matrix_multiply(
        upper_cross,
        first_size,
        second_size,
        second_solution,
        right_columns
    );
    for(std::size_t index = 0; index < first_rhs.size(); ++index){
        first_rhs[index] -= contribution[index];
    }
    std::vector<T> first_solution = left_solve_upper(
        upper_first, first_size, first_rhs, right_columns
    );
    first_solution.insert(
        first_solution.end(), second_solution.begin(), second_solution.end()
    );
    return first_solution;
}

template<class T>
GaussianLinearSystemSolution<T> solve(
    const std::vector<T>& matrix,
    std::size_t rows,
    std::size_t columns,
    const std::vector<T>& right_hand_side
){
    check_flat_shape(matrix, rows, columns);
    check_solution_column_count(columns);
    if(right_hand_side.size() != rows)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: right-hand-side shape violation "
            "(fast_gaussian_elimination)."
        );
    }
    if(columns == std::numeric_limits<std::size_t>::max())[[unlikely]]{
        throw std::length_error(
            "library assertion fault: size overflow "
            "(fast_gaussian_elimination)."
        );
    }

    const std::vector<std::size_t> pivot_columns =
        column_rank_profile(matrix, rows, columns);
    const std::size_t rank = pivot_columns.size();
    std::vector<unsigned char> is_pivot(columns, 0);
    for(const std::size_t column: pivot_columns) is_pivot[column] = 1;
    std::vector<std::size_t> free_columns;
    free_columns.reserve(columns - rank);
    for(std::size_t column = 0; column < columns; ++column){
        if(is_pivot[column] == 0) free_columns.push_back(column);
    }
    std::vector<std::size_t> column_order(pivot_columns);
    column_order.insert(
        column_order.end(), free_columns.begin(), free_columns.end()
    );

    const std::size_t width = columns + 1;
    std::vector<T> augmented(checked_area(rows, width));
    for(std::size_t row = 0; row < rows; ++row){
        for(std::size_t column = 0; column < columns; ++column){
            augmented[row * width + column] =
                matrix[row * columns + column_order[column]];
        }
        augmented[row * width + columns] = right_hand_side[row];
    }
    std::vector<std::size_t> row_indices(rows);
    std::iota(row_indices.begin(), row_indices.end(), std::size_t(0));
    const auto profile =
        fast_characteristic_polynomial_internal::ordered_row_echelon(
            augmented, rows, width, row_indices
        );

    GaussianLinearSystemSolution<T> result;
    if(profile.rank() != rank) return result;
    result.consistent = true;
    result.pivot_columns.reserve(rank);
    for(const std::size_t column: pivot_columns){
        result.pivot_columns.push_back(static_cast<int>(column));
    }
    result.particular.assign(columns, T{});
    (void)checked_area(free_columns.size(), columns);
    result.nullspace_basis.assign(
        free_columns.size(), std::vector<T>(columns, T{})
    );
    if(rank == 0){
        for(std::size_t index = 0; index < free_columns.size(); ++index){
            result.nullspace_basis[index][free_columns[index]] = T(1);
        }
        return result;
    }

    std::vector<std::size_t> permuted_position(width);
    for(std::size_t position = 0; position < width; ++position){
        permuted_position[profile.column_permutation[position]] = position;
    }
    const std::size_t rhs_position = permuted_position[columns];
    if(rhs_position < rank)[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: inconsistent rank profile "
            "(fast_gaussian_elimination)."
        );
    }
    std::vector<T> upper(checked_area(rank, rank));
    const std::size_t right_columns = free_columns.size() + 1;
    std::vector<T> batched_rhs(checked_area(rank, right_columns));
    for(std::size_t row = 0; row < rank; ++row){
        for(std::size_t column = 0; column < rank; ++column){
            upper[row * rank + column] =
                profile.upper[row * width + column];
        }
        batched_rhs[row * right_columns] =
            profile.upper[row * width + rhs_position];
        for(std::size_t index = 0; index < free_columns.size(); ++index){
            const std::size_t reordered_column = rank + index;
            const std::size_t position = permuted_position[reordered_column];
            if(position < rank)[[unlikely]]{
                throw std::logic_error(
                    "library assertion fault: inconsistent pivot profile "
                    "(fast_gaussian_elimination)."
                );
            }
            batched_rhs[row * right_columns + 1 + index] =
                -profile.upper[row * width + position];
        }
    }
    const std::vector<T> pivot_values = left_solve_upper(
        upper, rank, batched_rhs, right_columns
    );
    for(std::size_t position = 0; position < rank; ++position){
        const std::size_t reordered_column =
            profile.column_permutation[position];
        if(reordered_column >= rank)[[unlikely]]{
            throw std::logic_error(
                "library assertion fault: inconsistent pivot profile "
                "(fast_gaussian_elimination)."
            );
        }
        const std::size_t original_column = column_order[reordered_column];
        result.particular[original_column] =
            pivot_values[position * right_columns];
        for(std::size_t index = 0; index < free_columns.size(); ++index){
            result.nullspace_basis[index][original_column] =
                pivot_values[position * right_columns + 1 + index];
        }
    }
    for(std::size_t index = 0; index < free_columns.size(); ++index){
        result.nullspace_basis[index][free_columns[index]] = T(1);
    }
    return result;
}

}  // namespace fast_gaussian_elimination_internal

template<class T>
std::size_t fast_gaussian_rank(
    const std::vector<T>& matrix,
    std::size_t rows,
    std::size_t columns
){
    return fast_gaussian_elimination_internal::column_rank_profile(
        matrix, rows, columns
    ).size();
}

template<class T>
std::size_t fast_gaussian_rank(const std::vector<std::vector<T>>& matrix){
    const auto [rows, columns] =
        fast_gaussian_elimination_internal::nested_shape(matrix);
    return fast_gaussian_rank(
        fast_gaussian_elimination_internal::flatten(matrix), rows, columns
    );
}

template<class T, int MAX_ROW, int MAX_COL>
std::size_t fast_gaussian_rank(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
){
    return fast_gaussian_rank(
        fast_gaussian_elimination_internal::flatten(matrix),
        static_cast<std::size_t>(matrix.rows()),
        static_cast<std::size_t>(matrix.cols())
    );
}

template<class T>
GaussianLinearSystemSolution<T> fast_solve_linear_system(
    const std::vector<T>& matrix,
    std::size_t rows,
    std::size_t columns,
    const std::vector<T>& right_hand_side
){
    return fast_gaussian_elimination_internal::solve(
        matrix, rows, columns, right_hand_side
    );
}

template<class T>
GaussianLinearSystemSolution<T> fast_solve_linear_system(
    const std::vector<std::vector<T>>& matrix,
    const std::vector<T>& right_hand_side
){
    const auto [rows, columns] =
        fast_gaussian_elimination_internal::nested_shape(matrix);
    return fast_solve_linear_system(
        fast_gaussian_elimination_internal::flatten(matrix),
        rows,
        columns,
        right_hand_side
    );
}

template<class T, int MAX_ROW, int MAX_COL>
GaussianLinearSystemSolution<T> fast_solve_linear_system(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix,
    const std::vector<T>& right_hand_side
){
    return fast_solve_linear_system(
        fast_gaussian_elimination_internal::flatten(matrix),
        static_cast<std::size_t>(matrix.rows()),
        static_cast<std::size_t>(matrix.cols()),
        right_hand_side
    );
}

template<class T>
std::optional<std::vector<T>> fast_gaussian_inverse(
    const std::vector<T>& matrix,
    std::size_t size
){
    fast_gaussian_elimination_internal::check_flat_shape(matrix, size, size);
    if(fast_gaussian_rank(matrix, size, size) != size) return std::nullopt;
    return fast_characteristic_polynomial_internal::fast_inverse(matrix, size);
}

template<class T>
std::optional<std::vector<std::vector<T>>> fast_gaussian_inverse(
    const std::vector<std::vector<T>>& matrix
){
    const auto [rows, columns] =
        fast_gaussian_elimination_internal::nested_shape(matrix);
    if(rows != columns)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: square violation "
            "(fast_gaussian_inverse)."
        );
    }
    const auto inverse = fast_gaussian_inverse(
        fast_gaussian_elimination_internal::flatten(matrix), rows
    );
    if(!inverse) return std::nullopt;
    std::vector<std::vector<T>> result(
        rows, std::vector<T>(columns)
    );
    for(std::size_t row = 0; row < rows; ++row){
        std::copy_n(
            inverse->begin() + static_cast<std::ptrdiff_t>(row * columns),
            columns,
            result[row].begin()
        );
    }
    return result;
}

template<class T, int MAX_ROW, int MAX_COL>
std::optional<Matrix<T, MAX_ROW, MAX_COL>> fast_gaussian_inverse(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: square violation "
            "(fast_gaussian_inverse)."
        );
    }
    const std::size_t size = static_cast<std::size_t>(matrix.rows());
    const auto inverse = fast_gaussian_inverse(
        fast_gaussian_elimination_internal::flatten(matrix), size
    );
    if(!inverse) return std::nullopt;
    Matrix<T, MAX_ROW, MAX_COL> result(matrix.rows(), matrix.cols());
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            result(static_cast<int>(row), static_cast<int>(column)) =
                (*inverse)[row * size + column];
        }
    }
    return result;
}

template<class T>
T fast_gaussian_determinant(
    const std::vector<T>& matrix,
    std::size_t size
){
    return fast_determinant(matrix, size);
}

template<class T>
T fast_gaussian_determinant(
    const std::vector<std::vector<T>>& matrix
){
    return fast_determinant(matrix);
}

template<class T, int MAX_ROW, int MAX_COL>
T fast_gaussian_determinant(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
){
    return fast_determinant(matrix);
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_FAST_GAUSSIAN_ELIMINATION_HPP_INCLUDED
