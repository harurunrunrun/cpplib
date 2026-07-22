#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_CHARACTERISTIC_POLYNOMIAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_CHARACTERISTIC_POLYNOMIAL_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "fast_matrix_multiply.hpp"

namespace fast_characteristic_polynomial_internal{

inline std::size_t checked_area(std::size_t rows, std::size_t columns){
    if(rows != 0 && columns > std::numeric_limits<std::size_t>::max() / rows){
        throw std::length_error(
            "library assertion fault: size overflow "
            "(fast_characteristic_polynomial)."
        );
    }
    return rows * columns;
}

template<class T>
struct row_echelon_result{
    std::size_t columns = 0;
    std::vector<T> upper;
    std::vector<std::size_t> column_permutation;
    std::vector<std::size_t> pivot_rows;

    std::size_t rank() const{
        return pivot_rows.size();
    }
};

template<class T>
std::vector<T> right_solve_upper(
    const std::vector<T>& right_hand_side,
    std::size_t rows,
    const std::vector<T>& upper,
    std::size_t size
){
    if(right_hand_side.size() != rows * size ||
        upper.size() != size * size
    )[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: internal shape violation "
            "(fast_characteristic_polynomial)."
        );
    }
    if(rows == 0 || size == 0) return right_hand_side;
    if(size <= 32){
        std::vector<T> solution(right_hand_side);
        for(std::size_t column = 0; column < size; ++column){
            const T diagonal = upper[column * size + column];
            if(diagonal == T{})[[unlikely]]{
                throw std::logic_error(
                    "library assertion fault: zero pivot "
                    "(fast_characteristic_polynomial)."
                );
            }
            for(std::size_t row = 0; row < rows; ++row){
                T value = solution[row * size + column];
                for(std::size_t previous = 0; previous < column; ++previous){
                    value -= solution[row * size + previous] *
                        upper[previous * size + column];
                }
                solution[row * size + column] = value / diagonal;
            }
        }
        return solution;
    }

    const std::size_t left_size = size / 2;
    const std::size_t right_size = size - left_size;
    std::vector<T> first_rhs(rows * left_size);
    std::vector<T> second_rhs(rows * right_size);
    for(std::size_t row = 0; row < rows; ++row){
        for(std::size_t column = 0; column < left_size; ++column){
            first_rhs[row * left_size + column] =
                right_hand_side[row * size + column];
        }
        for(std::size_t column = 0; column < right_size; ++column){
            second_rhs[row * right_size + column] =
                right_hand_side[row * size + left_size + column];
        }
    }
    std::vector<T> upper_left(left_size * left_size);
    std::vector<T> upper_right(left_size * right_size);
    std::vector<T> lower_right(right_size * right_size);
    for(std::size_t row = 0; row < left_size; ++row){
        for(std::size_t column = 0; column < left_size; ++column){
            upper_left[row * left_size + column] = upper[row * size + column];
        }
        for(std::size_t column = 0; column < right_size; ++column){
            upper_right[row * right_size + column] =
                upper[row * size + left_size + column];
        }
    }
    for(std::size_t row = 0; row < right_size; ++row){
        for(std::size_t column = 0; column < right_size; ++column){
            lower_right[row * right_size + column] =
                upper[(left_size + row) * size + left_size + column];
        }
    }

    std::vector<T> first_solution = right_solve_upper(
        first_rhs, rows, upper_left, left_size
    );
    const std::vector<T> contribution = fast_matrix_multiply(
        first_solution, rows, left_size, upper_right, right_size
    );
    for(std::size_t index = 0; index < second_rhs.size(); ++index){
        second_rhs[index] -= contribution[index];
    }
    std::vector<T> second_solution = right_solve_upper(
        second_rhs, rows, lower_right, right_size
    );

    std::vector<T> solution(rows * size);
    for(std::size_t row = 0; row < rows; ++row){
        std::copy_n(
            first_solution.data() + row * left_size,
            left_size,
            solution.data() + row * size
        );
        std::copy_n(
            second_solution.data() + row * right_size,
            right_size,
            solution.data() + row * size + left_size
        );
    }
    return solution;
}

template<class T>
row_echelon_result<T> ordered_row_echelon(
    const std::vector<T>& values,
    std::size_t rows,
    std::size_t columns,
    const std::vector<std::size_t>& row_indices
){
    if(values.size() != rows * columns || row_indices.size() != rows)[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: internal shape violation "
            "(fast_characteristic_polynomial)."
        );
    }
    row_echelon_result<T> result;
    result.columns = columns;
    result.column_permutation.resize(columns);
    std::iota(
        result.column_permutation.begin(),
        result.column_permutation.end(),
        std::size_t(0)
    );
    if(rows == 0 || columns == 0) return result;

    if(rows == 1){
        std::size_t pivot = 0;
        while(pivot < columns && values[pivot] == T{}) ++pivot;
        if(pivot == columns) return result;
        std::swap(
            result.column_permutation[0],
            result.column_permutation[pivot]
        );
        result.upper.resize(columns);
        for(std::size_t column = 0; column < columns; ++column){
            result.upper[column] = values[result.column_permutation[column]];
        }
        result.pivot_rows.push_back(row_indices[0]);
        return result;
    }

    const std::size_t upper_rows = rows / 2;
    const std::size_t lower_rows = rows - upper_rows;
    std::vector<T> upper_values(upper_rows * columns);
    std::copy_n(values.begin(), upper_values.size(), upper_values.begin());
    const std::vector<std::size_t> upper_indices(
        row_indices.begin(),
        row_indices.begin() + static_cast<std::ptrdiff_t>(upper_rows)
    );
    row_echelon_result<T> first = ordered_row_echelon(
        upper_values, upper_rows, columns, upper_indices
    );
    const std::size_t first_rank = first.rank();
    const std::size_t remaining_columns = columns - first_rank;

    std::vector<T> reordered_lower(lower_rows * columns);
    for(std::size_t row = 0; row < lower_rows; ++row){
        for(std::size_t column = 0; column < columns; ++column){
            reordered_lower[row * columns + column] = values[
                (upper_rows + row) * columns +
                first.column_permutation[column]
            ];
        }
    }

    std::vector<T> residual(lower_rows * remaining_columns);
    if(first_rank == 0){
        residual = std::move(reordered_lower);
    }else if(remaining_columns != 0){
        std::vector<T> upper_pivot(first_rank * first_rank);
        std::vector<T> upper_tail(first_rank * remaining_columns);
        for(std::size_t row = 0; row < first_rank; ++row){
            for(std::size_t column = 0; column < first_rank; ++column){
                upper_pivot[row * first_rank + column] =
                    first.upper[row * columns + column];
            }
            for(std::size_t column = 0; column < remaining_columns; ++column){
                upper_tail[row * remaining_columns + column] =
                    first.upper[row * columns + first_rank + column];
            }
        }
        std::vector<T> lower_pivot(lower_rows * first_rank);
        for(std::size_t row = 0; row < lower_rows; ++row){
            for(std::size_t column = 0; column < first_rank; ++column){
                lower_pivot[row * first_rank + column] =
                    reordered_lower[row * columns + column];
            }
        }
        const std::vector<T> multipliers = right_solve_upper(
            lower_pivot, lower_rows, upper_pivot, first_rank
        );
        const std::vector<T> eliminated = fast_matrix_multiply(
            multipliers,
            lower_rows,
            first_rank,
            upper_tail,
            remaining_columns
        );
        for(std::size_t row = 0; row < lower_rows; ++row){
            for(std::size_t column = 0; column < remaining_columns; ++column){
                residual[row * remaining_columns + column] =
                    reordered_lower[row * columns + first_rank + column] -
                    eliminated[row * remaining_columns + column];
            }
        }
    }

    const std::vector<std::size_t> lower_indices(
        row_indices.begin() + static_cast<std::ptrdiff_t>(upper_rows),
        row_indices.end()
    );
    row_echelon_result<T> second = ordered_row_echelon(
        residual, lower_rows, remaining_columns, lower_indices
    );
    const std::size_t second_rank = second.rank();

    result.pivot_rows = std::move(first.pivot_rows);
    result.pivot_rows.insert(
        result.pivot_rows.end(),
        second.pivot_rows.begin(),
        second.pivot_rows.end()
    );
    result.column_permutation.resize(columns);
    for(std::size_t column = 0; column < first_rank; ++column){
        result.column_permutation[column] =
            first.column_permutation[column];
    }
    for(std::size_t column = 0; column < remaining_columns; ++column){
        result.column_permutation[first_rank + column] =
            first.column_permutation[first_rank +
                second.column_permutation[column]];
    }

    result.upper.assign((first_rank + second_rank) * columns, T{});
    for(std::size_t row = 0; row < first_rank; ++row){
        for(std::size_t column = 0; column < first_rank; ++column){
            result.upper[row * columns + column] =
                first.upper[row * columns + column];
        }
        for(std::size_t column = 0; column < remaining_columns; ++column){
            result.upper[row * columns + first_rank + column] =
                first.upper[row * columns + first_rank +
                    second.column_permutation[column]];
        }
    }
    for(std::size_t row = 0; row < second_rank; ++row){
        for(std::size_t column = 0; column < remaining_columns; ++column){
            result.upper[(first_rank + row) * columns + first_rank + column] =
                second.upper[row * remaining_columns + column];
        }
    }
    return result;
}

template<class T>
std::vector<T> classical_inverse(
    const std::vector<T>& matrix,
    std::size_t size
){
    std::vector<T> augmented(size * (size * 2), T{});
    const std::size_t stride = size * 2;
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            augmented[row * stride + column] = matrix[row * size + column];
        }
        augmented[row * stride + size + row] = T(1);
    }
    for(std::size_t column = 0; column < size; ++column){
        std::size_t pivot = column;
        while(pivot < size && augmented[pivot * stride + column] == T{}){
            ++pivot;
        }
        if(pivot == size)[[unlikely]]{
            throw std::logic_error(
                "library assertion fault: singular Krylov basis "
                "(fast_characteristic_polynomial)."
            );
        }
        if(pivot != column){
            for(std::size_t entry = 0; entry < stride; ++entry){
                std::swap(
                    augmented[pivot * stride + entry],
                    augmented[column * stride + entry]
                );
            }
        }
        const T inverse = T(1) / augmented[column * stride + column];
        for(std::size_t entry = 0; entry < stride; ++entry){
            augmented[column * stride + entry] *= inverse;
        }
        for(std::size_t row = 0; row < size; ++row){
            if(row == column) continue;
            const T factor = augmented[row * stride + column];
            if(factor == T{}) continue;
            for(std::size_t entry = 0; entry < stride; ++entry){
                augmented[row * stride + entry] -=
                    factor * augmented[column * stride + entry];
            }
        }
    }
    std::vector<T> inverse(size * size);
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            inverse[row * size + column] =
                augmented[row * stride + size + column];
        }
    }
    return inverse;
}

template<class T>
std::vector<T> fast_inverse(
    const std::vector<T>& matrix,
    std::size_t size
){
    if(matrix.size() != size * size)[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: internal shape violation "
            "(fast_characteristic_polynomial)."
        );
    }
    if(size == 0) return {};
    if(size <= 32) return classical_inverse(matrix, size);

    const std::size_t first_size = size / 2;
    const std::size_t second_size = size - first_size;
    std::vector<T> first_rows(first_size * size);
    std::copy_n(matrix.begin(), first_rows.size(), first_rows.begin());
    std::vector<std::size_t> row_indices(first_size);
    std::iota(row_indices.begin(), row_indices.end(), std::size_t(0));
    const row_echelon_result<T> profile = ordered_row_echelon(
        first_rows, first_size, size, row_indices
    );
    if(profile.rank() != first_size)[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: singular Krylov basis "
            "(fast_characteristic_polynomial)."
        );
    }

    std::vector<T> permuted(size * size);
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            permuted[row * size + column] = matrix[
                row * size + profile.column_permutation[column]
            ];
        }
    }
    std::vector<T> first_first(first_size * first_size);
    std::vector<T> first_second(first_size * second_size);
    std::vector<T> second_first(second_size * first_size);
    std::vector<T> second_second(second_size * second_size);
    for(std::size_t row = 0; row < first_size; ++row){
        for(std::size_t column = 0; column < first_size; ++column){
            first_first[row * first_size + column] =
                permuted[row * size + column];
        }
        for(std::size_t column = 0; column < second_size; ++column){
            first_second[row * second_size + column] =
                permuted[row * size + first_size + column];
        }
    }
    for(std::size_t row = 0; row < second_size; ++row){
        for(std::size_t column = 0; column < first_size; ++column){
            second_first[row * first_size + column] =
                permuted[(first_size + row) * size + column];
        }
        for(std::size_t column = 0; column < second_size; ++column){
            second_second[row * second_size + column] =
                permuted[(first_size + row) * size + first_size + column];
        }
    }

    std::vector<T> inverse_first = fast_inverse(first_first, first_size);
    const std::vector<T> x = fast_matrix_multiply(
        inverse_first,
        first_size,
        first_size,
        first_second,
        second_size
    );
    const std::vector<T> y = fast_matrix_multiply(
        second_first,
        second_size,
        first_size,
        inverse_first,
        first_size
    );
    const std::vector<T> correction = fast_matrix_multiply(
        second_first,
        second_size,
        first_size,
        x,
        second_size
    );
    for(std::size_t index = 0; index < second_second.size(); ++index){
        second_second[index] -= correction[index];
    }
    const std::vector<T> inverse_second = fast_inverse(
        second_second, second_size
    );

    std::vector<T> top_right = fast_matrix_multiply(
        x,
        first_size,
        second_size,
        inverse_second,
        second_size
    );
    std::vector<T> bottom_left = fast_matrix_multiply(
        inverse_second,
        second_size,
        second_size,
        y,
        first_size
    );
    for(T& value: top_right) value = -value;
    for(T& value: bottom_left) value = -value;
    const std::vector<T> top_correction = fast_matrix_multiply(
        x,
        first_size,
        second_size,
        bottom_left,
        first_size
    );
    for(std::size_t index = 0; index < inverse_first.size(); ++index){
        inverse_first[index] -= top_correction[index];
    }

    std::vector<T> inverse_permuted(size * size);
    for(std::size_t row = 0; row < first_size; ++row){
        for(std::size_t column = 0; column < first_size; ++column){
            inverse_permuted[row * size + column] =
                inverse_first[row * first_size + column];
        }
        for(std::size_t column = 0; column < second_size; ++column){
            inverse_permuted[row * size + first_size + column] =
                top_right[row * second_size + column];
        }
    }
    for(std::size_t row = 0; row < second_size; ++row){
        for(std::size_t column = 0; column < first_size; ++column){
            inverse_permuted[(first_size + row) * size + column] =
                bottom_left[row * first_size + column];
        }
        for(std::size_t column = 0; column < second_size; ++column){
            inverse_permuted[(first_size + row) * size + first_size + column] =
                inverse_second[row * second_size + column];
        }
    }

    std::vector<T> inverse(size * size);
    for(std::size_t new_row = 0; new_row < size; ++new_row){
        const std::size_t old_row = profile.column_permutation[new_row];
        std::copy_n(
            inverse_permuted.data() + new_row * size,
            size,
            inverse.data() + old_row * size
        );
    }
    return inverse;
}

template<class T>
std::vector<T> multiply_polynomials(
    const std::vector<T>& left,
    const std::vector<T>& right
){
    std::vector<T> product(left.size() + right.size() - 1, T{});
    for(std::size_t i = 0; i < left.size(); ++i){
        for(std::size_t j = 0; j < right.size(); ++j){
            product[i + j] += left[i] * right[j];
        }
    }
    return product;
}

template<class T>
std::vector<T> compute(std::vector<T> matrix, std::size_t size){
    if(size == 0) return {T(1)};
    std::vector<T> basis(size * size, T{});
    for(std::size_t index = 0; index < size; ++index){
        basis[index * size + index] = T(1);
    }
    std::vector<std::size_t> chain_lengths(size, 1);
    std::size_t block_size = 1;
    std::vector<T> power = matrix;

    while(true){
        const std::vector<T> powered_basis = fast_matrix_multiply(
            power, size, size, basis, size
        );
        std::size_t candidate_count = 0;
        for(const std::size_t length: chain_lengths){
            candidate_count += length;
            if(length == block_size) candidate_count += length;
        }
        std::vector<T> candidates(size * candidate_count);
        std::vector<std::size_t> block_offsets;
        block_offsets.reserve(chain_lengths.size() + 1);
        block_offsets.push_back(0);
        std::size_t basis_column = 0;
        std::size_t candidate_column = 0;
        for(const std::size_t length: chain_lengths){
            for(std::size_t local = 0; local < length; ++local){
                for(std::size_t row = 0; row < size; ++row){
                    candidates[row * candidate_count + candidate_column] =
                        basis[row * size + basis_column + local];
                }
                ++candidate_column;
            }
            if(length == block_size){
                for(std::size_t local = 0; local < length; ++local){
                    for(std::size_t row = 0; row < size; ++row){
                        candidates[row * candidate_count + candidate_column] =
                            powered_basis[row * size + basis_column + local];
                    }
                    ++candidate_column;
                }
            }
            basis_column += length;
            block_offsets.push_back(candidate_column);
        }

        std::vector<T> transposed(candidate_count * size);
        for(std::size_t row = 0; row < size; ++row){
            for(std::size_t column = 0; column < candidate_count; ++column){
                transposed[column * size + row] =
                    candidates[row * candidate_count + column];
            }
        }
        std::vector<std::size_t> candidate_indices(candidate_count);
        std::iota(
            candidate_indices.begin(),
            candidate_indices.end(),
            std::size_t(0)
        );
        const row_echelon_result<T> profile = ordered_row_echelon(
            transposed, candidate_count, size, candidate_indices
        );
        if(profile.rank() != size)[[unlikely]]{
            throw std::logic_error(
                "library assertion fault: incomplete Krylov basis "
                "(fast_characteristic_polynomial)."
            );
        }
        std::vector<unsigned char> selected(candidate_count, 0);
        for(const std::size_t index: profile.pivot_rows) selected[index] = 1;

        std::vector<T> next_basis(size * size);
        std::vector<std::size_t> next_lengths;
        next_lengths.reserve(chain_lengths.size());
        std::size_t next_column = 0;
        for(std::size_t chain = 0; chain < chain_lengths.size(); ++chain){
            const std::size_t begin = block_offsets[chain];
            const std::size_t end = block_offsets[chain + 1];
            // A profile can contain gaps while an earlier full chain is
            // still growing.  Preserve the selected vectors themselves and
            // split them into maximal consecutive Krylov runs; only a fully
            // selected run reaches the next doubling length.
            std::size_t index = begin;
            while(index < end){
                while(index < end && selected[index] == 0) ++index;
                if(index == end) break;
                const std::size_t run_begin = index;
                while(index < end && selected[index] != 0) ++index;
                const std::size_t run_length = index - run_begin;
                next_lengths.push_back(run_length);
                for(std::size_t local = 0; local < run_length; ++local){
                    for(std::size_t row = 0; row < size; ++row){
                        next_basis[row * size + next_column + local] =
                            candidates[
                                row * candidate_count + run_begin + local
                            ];
                    }
                }
                next_column += run_length;
            }
        }
        if(next_column != size)[[unlikely]]{
            throw std::logic_error(
                "library assertion fault: incomplete Krylov basis "
                "(fast_characteristic_polynomial)."
            );
        }
        basis = std::move(next_basis);
        chain_lengths = std::move(next_lengths);

        if(block_size > std::numeric_limits<std::size_t>::max() / 2)[[unlikely]]{
            throw std::length_error(
                "library assertion fault: size overflow "
                "(fast_characteristic_polynomial)."
            );
        }
        const std::size_t next_block_size = block_size * 2;
        bool continue_doubling = false;
        for(const std::size_t length: chain_lengths){
            if(length == next_block_size){
                continue_doubling = true;
                break;
            }
        }
        if(!continue_doubling) break;
        block_size = next_block_size;
        power = fast_matrix_multiply(power, size, size, power, size);
    }

    const std::vector<T> inverse_basis = fast_inverse(basis, size);
    const std::vector<T> image_basis = fast_matrix_multiply(
        matrix, size, size, basis, size
    );
    const std::vector<T> representation = fast_matrix_multiply(
        inverse_basis, size, size, image_basis, size
    );

    std::vector<T> characteristic{T(1)};
    std::size_t chain_begin = 0;
    for(const std::size_t length: chain_lengths){
        const std::size_t chain_end = chain_begin + length;
        for(std::size_t column = chain_begin; column + 1 < chain_end; ++column){
            for(std::size_t row = 0; row < size; ++row){
                const T expected = row == column + 1 ? T(1) : T{};
                if(representation[row * size + column] != expected)[[unlikely]]{
                    throw std::logic_error(
                        "library assertion fault: invalid Krylov shift "
                        "(fast_characteristic_polynomial)."
                    );
                }
            }
        }
        const std::size_t last_column = chain_end - 1;
        for(std::size_t row = chain_end; row < size; ++row){
            if(representation[row * size + last_column] != T{})[[unlikely]]{
                throw std::logic_error(
                    "library assertion fault: invalid Krylov dependency "
                    "(fast_characteristic_polynomial)."
                );
            }
        }
        std::vector<T> factor(length + 1, T{});
        factor[length] = T(1);
        for(std::size_t degree = 0; degree < length; ++degree){
            factor[degree] = -representation[
                (chain_begin + degree) * size + last_column
            ];
        }
        characteristic = multiply_polynomials(characteristic, factor);
        chain_begin = chain_end;
    }
    return characteristic;
}

}  // namespace fast_characteristic_polynomial_internal

template<class T>
std::vector<T> fast_characteristic_polynomial(
    const std::vector<std::vector<T>>& matrix
){
    const std::size_t size = matrix.size();
    std::vector<T> values(
        fast_characteristic_polynomial_internal::checked_area(size, size)
    );
    for(std::size_t row = 0; row < size; ++row){
        if(matrix[row].size() != size)[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: square violation "
                "(fast_characteristic_polynomial)."
            );
        }
        for(std::size_t column = 0; column < size; ++column){
            values[row * size + column] = matrix[row][column];
        }
    }
    return fast_characteristic_polynomial_internal::compute(
        std::move(values), size
    );
}

template<class T, int MAX_ROW, int MAX_COL>
std::vector<T> fast_characteristic_polynomial(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: square violation "
            "(fast_characteristic_polynomial)."
        );
    }
    const std::size_t size = static_cast<std::size_t>(matrix.rows());
    std::vector<T> values(
        fast_characteristic_polynomial_internal::checked_area(size, size)
    );
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            values[row * size + column] = matrix(
                static_cast<int>(row), static_cast<int>(column)
            );
        }
    }
    return fast_characteristic_polynomial_internal::compute(
        std::move(values), size
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_CHARACTERISTIC_POLYNOMIAL_HPP_INCLUDED
