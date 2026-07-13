#pragma once

#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include "../../structure/matrix/matrix.hpp"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

template<class T>
std::vector<std::vector<T>> adjugate_matrix(
    const std::vector<std::vector<T>>& matrix
){
    const std::size_t size = matrix.size();
    for(const auto& row: matrix){
        if(row.size() != size)[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: square violation (adjugate_matrix)."
            );
        }
    }
    if(size == 0) return {};

    std::vector<T> reduced(size * size);
    std::vector<T> transform(size * size);
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            reduced[row * size + column] = matrix[row][column];
        }
        transform[row * size + row] = T(1);
    }
    std::vector<std::size_t> row_ids(size);
    std::iota(row_ids.begin(), row_ids.end(), std::size_t{0});
    std::vector<std::size_t> pivot_columns;
    pivot_columns.reserve(size);
    T pivot_product = T(1);

    for(std::size_t column = 0; column < size; ++column){
        const std::size_t rank = pivot_columns.size();
        std::size_t pivot = rank;
        while(pivot < size && reduced[pivot * size + column] == T{}) ++pivot;
        if(pivot == size) continue;
        if(pivot != rank){
            for(std::size_t entry = 0; entry < size; ++entry){
                std::swap(
                    reduced[pivot * size + entry],
                    reduced[rank * size + entry]
                );
                std::swap(
                    transform[pivot * size + entry],
                    transform[rank * size + entry]
                );
            }
            std::swap(row_ids[pivot], row_ids[rank]);
        }

        const T pivot_value = reduced[rank * size + column];
        pivot_product *= pivot_value;
        const T inverse = T(1) / pivot_value;
        for(std::size_t entry = column; entry < size; ++entry){
            reduced[rank * size + entry] *= inverse;
        }
        for(std::size_t entry = 0; entry < size; ++entry){
            transform[rank * size + entry] *= inverse;
        }
        for(std::size_t row = rank + 1; row < size; ++row){
            const T factor = reduced[row * size + column];
            if(factor == T{}) continue;
            for(std::size_t entry = column; entry < size; ++entry){
                reduced[row * size + entry] -=
                    factor * reduced[rank * size + entry];
            }
            for(std::size_t entry = 0; entry < size; ++entry){
                transform[row * size + entry] -=
                    factor * transform[rank * size + entry];
            }
        }
        pivot_columns.push_back(column);
    }

    const std::size_t rank = pivot_columns.size();
    std::vector<std::vector<T>> result(size, std::vector<T>(size));
    if(rank + 1 < size) return result;

    for(std::size_t pivot_index = rank; pivot_index-- > 0; ){
        const std::size_t column = pivot_columns[pivot_index];
        for(std::size_t row = 0; row < pivot_index; ++row){
            const T factor = reduced[row * size + column];
            if(factor == T{}) continue;
            for(std::size_t entry = column; entry < size; ++entry){
                reduced[row * size + entry] -=
                    factor * reduced[pivot_index * size + entry];
            }
            for(std::size_t entry = 0; entry < size; ++entry){
                transform[row * size + entry] -=
                    factor * transform[pivot_index * size + entry];
            }
        }
    }

    bool negative_minor = false;
    for(std::size_t first = 0; first < rank; ++first){
        for(std::size_t second = first + 1; second < rank; ++second){
            if(row_ids[first] > row_ids[second]) negative_minor = !negative_minor;
        }
    }
    if(negative_minor) pivot_product = -pivot_product;

    if(rank == size){
        for(std::size_t row = 0; row < size; ++row){
            for(std::size_t column = 0; column < size; ++column){
                result[row][column] =
                    pivot_product * transform[row * size + column];
            }
        }
        return result;
    }

    std::vector<bool> is_pivot(size, false);
    for(const std::size_t column: pivot_columns) is_pivot[column] = true;
    std::size_t free_column = 0;
    while(is_pivot[free_column]) ++free_column;
    const std::size_t free_row = row_ids[rank];

    std::vector<T> right_kernel(size);
    right_kernel[free_column] = T(1);
    for(std::size_t pivot_index = 0; pivot_index < rank; ++pivot_index){
        right_kernel[pivot_columns[pivot_index]] =
            -reduced[pivot_index * size + free_column];
    }
    std::vector<T> left_kernel(size);
    const T left_scale = transform[rank * size + free_row];
    if(left_scale == T{})[[unlikely]]{
        throw std::logic_error(
            "library internal error: zero kernel normalization (adjugate_matrix)."
        );
    }
    const T left_inverse = T(1) / left_scale;
    for(std::size_t column = 0; column < size; ++column){
        left_kernel[column] = transform[rank * size + column] * left_inverse;
    }

    T scale = pivot_product;
    if(((free_row + free_column) & 1U) != 0U) scale = -scale;
    for(std::size_t row = 0; row < size; ++row){
        const T row_scale = scale * right_kernel[row];
        for(std::size_t column = 0; column < size; ++column){
            result[row][column] = row_scale * left_kernel[column];
        }
    }
    return result;
}

template<class T, int MAX_ROW, int MAX_COL>
Matrix<T, MAX_ROW, MAX_COL> adjugate_matrix(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: square violation (adjugate_matrix)."
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
    const auto result = adjugate_matrix(values);
    Matrix<T, MAX_ROW, MAX_COL> fixed_result(matrix.rows(), matrix.cols());
    for(int row = 0; row < matrix.rows(); ++row){
        for(int column = 0; column < matrix.cols(); ++column){
            fixed_result(row, column) = result[static_cast<std::size_t>(row)]
                [static_cast<std::size_t>(column)];
        }
    }
    return fixed_result;
}
