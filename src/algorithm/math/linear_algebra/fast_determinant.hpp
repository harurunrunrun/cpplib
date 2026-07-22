#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_DETERMINANT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_DETERMINANT_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "fast_characteristic_polynomial.hpp"

namespace fast_determinant_internal{

inline std::size_t checked_square(std::size_t size){
    if(size != 0 && size > std::numeric_limits<std::size_t>::max() / size){
        throw std::length_error(
            "library assertion fault: size overflow (fast_determinant)."
        );
    }
    return size * size;
}

inline bool odd_permutation(const std::vector<std::size_t>& permutation){
    std::vector<unsigned char> visited(permutation.size(), 0);
    std::size_t cycle_count = 0;
    for(std::size_t begin = 0; begin < permutation.size(); ++begin){
        if(visited[begin] != 0) continue;
        ++cycle_count;
        std::size_t index = begin;
        while(visited[index] == 0){
            visited[index] = 1;
            index = permutation[index];
        }
    }
    return ((permutation.size() - cycle_count) & std::size_t(1)) != 0;
}

template<class T>
T classical(std::vector<T> matrix, std::size_t size){
    T determinant = T(1);
    for(std::size_t column = 0; column < size; ++column){
        std::size_t pivot = column;
        while(pivot < size && matrix[pivot * size + column] == T{}) ++pivot;
        if(pivot == size) return T{};
        if(pivot != column){
            for(std::size_t entry = column; entry < size; ++entry){
                std::swap(
                    matrix[pivot * size + entry],
                    matrix[column * size + entry]
                );
            }
            determinant = -determinant;
        }
        const T pivot_value = matrix[column * size + column];
        determinant *= pivot_value;
        const T inverse = T(1) / pivot_value;
        for(std::size_t row = column + 1; row < size; ++row){
            const T factor = matrix[row * size + column] * inverse;
            matrix[row * size + column] = T{};
            for(std::size_t entry = column + 1; entry < size; ++entry){
                matrix[row * size + entry] -=
                    factor * matrix[column * size + entry];
            }
        }
    }
    return determinant;
}

template<class T>
T compute(const std::vector<T>& matrix, std::size_t size){
    if(size == 0) return T(1);
    if(size <= 32) return classical(matrix, size);

    const std::size_t first_size = size / 2;
    const std::size_t second_size = size - first_size;
    std::vector<T> first_rows(first_size * size);
    std::copy_n(matrix.begin(), first_rows.size(), first_rows.begin());
    std::vector<std::size_t> row_indices(first_size);
    std::iota(row_indices.begin(), row_indices.end(), std::size_t(0));
    const auto profile =
        fast_characteristic_polynomial_internal::ordered_row_echelon(
            first_rows, first_size, size, row_indices
        );
    if(profile.rank() != first_size) return T{};

    std::vector<T> first_first(first_size * first_size);
    std::vector<T> first_second(first_size * second_size);
    std::vector<T> second_first(second_size * first_size);
    std::vector<T> second_second(second_size * second_size);
    for(std::size_t row = 0; row < first_size; ++row){
        for(std::size_t column = 0; column < first_size; ++column){
            first_first[row * first_size + column] = matrix[
                row * size + profile.column_permutation[column]
            ];
        }
        for(std::size_t column = 0; column < second_size; ++column){
            first_second[row * second_size + column] = matrix[
                row * size + profile.column_permutation[first_size + column]
            ];
        }
    }
    for(std::size_t row = 0; row < second_size; ++row){
        for(std::size_t column = 0; column < first_size; ++column){
            second_first[row * first_size + column] = matrix[
                (first_size + row) * size +
                profile.column_permutation[column]
            ];
        }
        for(std::size_t column = 0; column < second_size; ++column){
            second_second[row * second_size + column] = matrix[
                (first_size + row) * size +
                profile.column_permutation[first_size + column]
            ];
        }
    }

    const T first_determinant = compute(first_first, first_size);
    if(first_determinant == T{})[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: invalid rank profile (fast_determinant)."
        );
    }
    const std::vector<T> inverse_first =
        fast_characteristic_polynomial_internal::fast_inverse(
            first_first, first_size
        );
    const std::vector<T> lower_multiplier = fast_matrix_multiply(
        second_first,
        second_size,
        first_size,
        inverse_first,
        first_size
    );
    const std::vector<T> correction = fast_matrix_multiply(
        lower_multiplier,
        second_size,
        first_size,
        first_second,
        second_size
    );
    for(std::size_t index = 0; index < second_second.size(); ++index){
        second_second[index] -= correction[index];
    }
    T result = first_determinant * compute(second_second, second_size);
    if(odd_permutation(profile.column_permutation)) result = -result;
    return result;
}

}  // namespace fast_determinant_internal

template<class T>
T fast_determinant(
    const std::vector<T>& matrix,
    std::size_t size
){
    if(matrix.size() != fast_determinant_internal::checked_square(size)){
        throw std::invalid_argument(
            "library assertion fault: shape violation (fast_determinant)."
        );
    }
    return fast_determinant_internal::compute(matrix, size);
}

template<class T>
T fast_determinant(const std::vector<std::vector<T>>& matrix){
    const std::size_t size = matrix.size();
    std::vector<T> values(fast_determinant_internal::checked_square(size));
    for(std::size_t row = 0; row < size; ++row){
        if(matrix[row].size() != size)[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: square violation (fast_determinant)."
            );
        }
        std::copy_n(
            matrix[row].begin(), size, values.begin() + row * size
        );
    }
    return fast_determinant_internal::compute(values, size);
}

template<class T, int MAX_ROW, int MAX_COL>
T fast_determinant(const Matrix<T, MAX_ROW, MAX_COL>& matrix){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: square violation (fast_determinant)."
        );
    }
    const std::size_t size = static_cast<std::size_t>(matrix.rows());
    std::vector<T> values(fast_determinant_internal::checked_square(size));
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            values[row * size + column] = matrix(
                static_cast<int>(row), static_cast<int>(column)
            );
        }
    }
    return fast_determinant_internal::compute(values, size);
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_DETERMINANT_HPP_INCLUDED
