#ifndef CPPLIB_SRC_ALGORITHM_MATH_FAST_ADJUGATE_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_FAST_ADJUGATE_MATRIX_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "fast_determinant.hpp"

namespace fast_adjugate_matrix_internal{

inline std::size_t checked_square(std::size_t size){
    if(size != 0 && size > std::numeric_limits<std::size_t>::max() / size){
        throw std::length_error(
            "library assertion fault: size overflow (fast_adjugate_matrix)."
        );
    }
    return size * size;
}

template<class T>
std::vector<T> transpose(
    const std::vector<T>& matrix,
    std::size_t size
){
    std::vector<T> result(matrix.size());
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            result[column * size + row] = matrix[row * size + column];
        }
    }
    return result;
}

template<class T>
fast_characteristic_polynomial_internal::row_echelon_result<T> rank_profile(
    const std::vector<T>& matrix,
    std::size_t size
){
    std::vector<std::size_t> row_indices(size);
    std::iota(row_indices.begin(), row_indices.end(), std::size_t(0));
    return fast_characteristic_polynomial_internal::ordered_row_echelon(
        matrix, size, size, row_indices
    );
}

template<class T>
std::vector<T> one_dimensional_right_kernel(
    const fast_characteristic_polynomial_internal::row_echelon_result<T>&
        profile,
    std::size_t size
){
    if(profile.rank() + 1 != size)[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: invalid rank profile "
            "(fast_adjugate_matrix)."
        );
    }
    std::vector<T> permuted(size, T{});
    permuted[size - 1] = T(1);
    for(std::size_t row = size - 1; row-- > 0; ){
        T value = -profile.upper[row * size + size - 1];
        for(std::size_t column = row + 1; column + 1 < size; ++column){
            value -= profile.upper[row * size + column] * permuted[column];
        }
        const T pivot = profile.upper[row * size + row];
        if(pivot == T{})[[unlikely]]{
            throw std::logic_error(
                "library assertion fault: zero rank-profile pivot "
                "(fast_adjugate_matrix)."
            );
        }
        permuted[row] = value / pivot;
    }
    std::vector<T> kernel(size);
    for(std::size_t column = 0; column < size; ++column){
        kernel[profile.column_permutation[column]] = permuted[column];
    }
    return kernel;
}

template<class T>
std::vector<T> deleted_minor(
    const std::vector<T>& matrix,
    std::size_t size,
    std::size_t deleted_row,
    std::size_t deleted_column
){
    const std::size_t minor_size = size - 1;
    std::vector<T> minor(checked_square(minor_size));
    std::size_t destination = 0;
    for(std::size_t row = 0; row < size; ++row){
        if(row == deleted_row) continue;
        for(std::size_t column = 0; column < size; ++column){
            if(column == deleted_column) continue;
            minor[destination] = matrix[row * size + column];
            ++destination;
        }
    }
    return minor;
}

template<class T>
std::vector<T> compute(
    const std::vector<T>& matrix,
    std::size_t size
){
    if(size == 0) return {};

    const auto right_profile = rank_profile(matrix, size);
    const std::size_t rank = right_profile.rank();
    std::vector<T> result(checked_square(size), T{});
    if(rank + 1 < size) return result;

    if(rank == size){
        const T determinant = fast_determinant(matrix, size);
        const std::vector<T> inverse =
            fast_characteristic_polynomial_internal::fast_inverse(
                matrix, size
            );
        for(std::size_t index = 0; index < result.size(); ++index){
            result[index] = determinant * inverse[index];
        }
        return result;
    }

    const std::vector<T> right_kernel =
        one_dimensional_right_kernel(right_profile, size);
    const auto left_profile = rank_profile(transpose(matrix, size), size);
    if(left_profile.rank() != rank)[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: transpose rank mismatch "
            "(fast_adjugate_matrix)."
        );
    }
    const std::vector<T> left_kernel =
        one_dimensional_right_kernel(left_profile, size);

    std::size_t result_row = 0;
    while(result_row < size && right_kernel[result_row] == T{}) ++result_row;
    std::size_t result_column = 0;
    while(result_column < size && left_kernel[result_column] == T{}){
        ++result_column;
    }
    if(result_row == size || result_column == size)[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: empty one-dimensional kernel "
            "(fast_adjugate_matrix)."
        );
    }

    // adj(A)[i,j] is the cofactor obtained by deleting row j and column i.
    const std::vector<T> minor = deleted_minor(
        matrix, size, result_column, result_row
    );
    T cofactor = fast_determinant(minor, size - 1);
    if(((result_row + result_column) & std::size_t(1)) != 0){
        cofactor = -cofactor;
    }
    if(cofactor == T{})[[unlikely]]{
        throw std::logic_error(
            "library assertion fault: zero rank-(n-1) cofactor "
            "(fast_adjugate_matrix)."
        );
    }
    const T scale = cofactor /
        (right_kernel[result_row] * left_kernel[result_column]);
    for(std::size_t row = 0; row < size; ++row){
        const T row_scale = scale * right_kernel[row];
        for(std::size_t column = 0; column < size; ++column){
            result[row * size + column] =
                row_scale * left_kernel[column];
        }
    }
    return result;
}

}  // namespace fast_adjugate_matrix_internal

template<class T>
std::vector<T> fast_adjugate_matrix(
    const std::vector<T>& matrix,
    std::size_t size
){
    if(matrix.size() != fast_adjugate_matrix_internal::checked_square(size)){
        throw std::invalid_argument(
            "library assertion fault: shape violation (fast_adjugate_matrix)."
        );
    }
    return fast_adjugate_matrix_internal::compute(matrix, size);
}

template<class T>
std::vector<std::vector<T>> fast_adjugate_matrix(
    const std::vector<std::vector<T>>& matrix
){
    const std::size_t size = matrix.size();
    std::vector<T> values(
        fast_adjugate_matrix_internal::checked_square(size)
    );
    for(std::size_t row = 0; row < size; ++row){
        if(matrix[row].size() != size)[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: square violation "
                "(fast_adjugate_matrix)."
            );
        }
        std::copy_n(
            matrix[row].begin(), size, values.begin() + row * size
        );
    }
    const std::vector<T> flat =
        fast_adjugate_matrix_internal::compute(values, size);
    std::vector<std::vector<T>> result(
        size, std::vector<T>(size)
    );
    for(std::size_t row = 0; row < size; ++row){
        std::copy_n(
            flat.begin() + static_cast<std::ptrdiff_t>(row * size),
            size,
            result[row].begin()
        );
    }
    return result;
}

template<class T, int MAX_ROW, int MAX_COL>
Matrix<T, MAX_ROW, MAX_COL> fast_adjugate_matrix(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: square violation "
            "(fast_adjugate_matrix)."
        );
    }
    const std::size_t size = static_cast<std::size_t>(matrix.rows());
    std::vector<T> values(
        fast_adjugate_matrix_internal::checked_square(size)
    );
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            values[row * size + column] = matrix(
                static_cast<int>(row), static_cast<int>(column)
            );
        }
    }
    const std::vector<T> flat =
        fast_adjugate_matrix_internal::compute(values, size);
    Matrix<T, MAX_ROW, MAX_COL> result(matrix.rows(), matrix.cols());
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            result(static_cast<int>(row), static_cast<int>(column)) =
                flat[row * size + column];
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_FAST_ADJUGATE_MATRIX_HPP_INCLUDED
