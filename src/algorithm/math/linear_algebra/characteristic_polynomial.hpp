#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_CHARACTERISTIC_POLYNOMIAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_CHARACTERISTIC_POLYNOMIAL_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../../structure/matrix/matrix.hpp"

template<class T>
std::vector<T> characteristic_polynomial(std::vector<std::vector<T>> matrix){
    const std::size_t size = matrix.size();
    for(const auto& row: matrix){
        if(row.size() != size)[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: square violation (characteristic_polynomial)."
            );
        }
    }
    if(size == 0) return {T(1)};

    std::vector<T> hessenberg(size * size);
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            hessenberg[row * size + column] =
                std::move(matrix[row][column]);
        }
    }

    for(std::size_t column = 0; column + 2 < size; ++column){
        std::size_t pivot = column + 1;
        while(pivot < size && hessenberg[pivot * size + column] == T{}) ++pivot;
        if(pivot == size) continue;
        if(pivot != column + 1){
            for(std::size_t entry = 0; entry < size; ++entry){
                std::swap(
                    hessenberg[pivot * size + entry],
                    hessenberg[(column + 1) * size + entry]
                );
            }
            for(std::size_t row = 0; row < size; ++row){
                std::swap(
                    hessenberg[row * size + pivot],
                    hessenberg[row * size + column + 1]
                );
            }
        }

        const T inverse = T(1) / hessenberg[(column + 1) * size + column];
        for(std::size_t row = column + 2; row < size; ++row){
            if(hessenberg[row * size + column] == T{}) continue;
            const T factor = hessenberg[row * size + column] * inverse;
            for(std::size_t entry = column; entry < size; ++entry){
                hessenberg[row * size + entry] -=
                    factor * hessenberg[(column + 1) * size + entry];
            }
            for(std::size_t entry = 0; entry < size; ++entry){
                hessenberg[entry * size + column + 1] +=
                    factor * hessenberg[entry * size + row];
            }
        }
    }

    const std::size_t stride = size + 1;
    std::vector<T> leading_polynomials(stride * stride);
    leading_polynomials[0] = T(1);
    for(std::size_t order = 1; order <= size; ++order){
        const std::size_t current = order * stride;
        const std::size_t previous = (order - 1) * stride;
        const T diagonal = hessenberg[(order - 1) * size + order - 1];
        for(std::size_t degree = 0; degree < order; ++degree){
            leading_polynomials[current + degree] -=
                leading_polynomials[previous + degree] * diagonal;
            leading_polynomials[current + degree + 1] +=
                leading_polynomials[previous + degree];
        }

        T subdiagonal_product = T(1);
        for(std::size_t distance = 1; distance < order; ++distance){
            const std::size_t submatrix_order = order - distance;
            subdiagonal_product *= hessenberg[
                submatrix_order * size + submatrix_order - 1
            ];
            const T factor = subdiagonal_product * hessenberg[
                (submatrix_order - 1) * size + order - 1
            ];
            if(factor == T{}) continue;
            const std::size_t source = (submatrix_order - 1) * stride;
            for(std::size_t degree = 0; degree < submatrix_order; ++degree){
                leading_polynomials[current + degree] -=
                    factor * leading_polynomials[source + degree];
            }
        }
    }

    return std::vector<T>(
        leading_polynomials.begin() + static_cast<std::ptrdiff_t>(size * stride),
        leading_polynomials.begin() + static_cast<std::ptrdiff_t>(size * stride + size + 1)
    );
}

template<class T, int MAX_ROW, int MAX_COL>
std::vector<T> characteristic_polynomial(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: square violation (characteristic_polynomial)."
        );
    }
    std::vector<std::vector<T>> values(
        static_cast<std::size_t>(matrix.rows()),
        std::vector<T>(static_cast<std::size_t>(matrix.cols()))
    );
    for(int row = 0; row < matrix.rows(); ++row){
        for(int column = 0; column < matrix.cols(); ++column){
            values[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] =
                matrix(row, column);
        }
    }
    return characteristic_polynomial(std::move(values));
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_CHARACTERISTIC_POLYNOMIAL_HPP_INCLUDED
