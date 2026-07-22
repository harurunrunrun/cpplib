#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_MATRIX_DETERMINANT_REAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_MATRIX_DETERMINANT_REAL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../query/gaussian_elimination_real.hpp"

inline long double matrix_determinant_real(
    const std::vector<std::vector<long double>>& input,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    using namespace geometry3d_linear_algebra_detail;
    const auto [rows, columns] = validate_real_matrix(input);
    validate_relative_epsilon(relative_epsilon);
    if(rows != columns){
        throw std::invalid_argument("determinant requires a square matrix");
    }
    if(rows == 0) return 1;
    const long double scale = maximum_absolute_entry(input);
    if(scale == 0) return 0;

    std::vector<std::vector<long double>> matrix = input;
    for(auto& row: matrix){
        for(long double& value: row) value /= scale;
    }
    int sign = 1;
    std::vector<long double> factors;
    factors.reserve(2 * rows);
    for(std::size_t column = 0; column < columns; ++column){
        std::size_t pivot = column;
        for(std::size_t row = column + 1; row < rows; ++row){
            if(std::abs(matrix[row][column]) > std::abs(matrix[pivot][column])){
                pivot = row;
            }
        }
        if(std::abs(matrix[pivot][column]) <= relative_epsilon) return 0;
        if(pivot != column){
            std::swap(matrix[pivot], matrix[column]);
            sign = -sign;
        }
        const long double pivot_value = matrix[column][column];
        factors.push_back(pivot_value);
        for(std::size_t row = column + 1; row < rows; ++row){
            const long double factor = matrix[row][column] / pivot_value;
            matrix[row][column] = 0;
            for(std::size_t current = column + 1; current < columns; ++current){
                matrix[row][current] -= factor * matrix[column][current];
                if(!std::isfinite(matrix[row][current])){
                    throw std::overflow_error("determinant elimination overflowed");
                }
            }
        }
    }

    long double mantissa = static_cast<long double>(sign);
    long long exponent = 0;
    const auto multiply_factor = [&](long double factor,
                                     long double& current_mantissa,
                                     long long& current_exponent){
        int factor_exponent = 0;
        const long double factor_mantissa = std::frexp(factor, &factor_exponent);
        current_mantissa *= factor_mantissa;
        int normalization_exponent = 0;
        current_mantissa = std::frexp(current_mantissa, &normalization_exponent);
        current_exponent += static_cast<long long>(factor_exponent) +
            normalization_exponent;
    };
    for(long double factor: factors){
        multiply_factor(factor, mantissa, exponent);
    }
    for(std::size_t count = 0; count < rows; ++count){
        multiply_factor(scale, mantissa, exponent);
    }

    if(exponent > std::numeric_limits<long double>::max_exponent){
        throw std::overflow_error("determinant is not representable");
    }
    if(exponent < std::numeric_limits<long double>::min_exponent -
        std::numeric_limits<long double>::digits){
        return std::copysign(0.0L, mantissa);
    }
    return std::ldexp(mantissa, static_cast<int>(exponent));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_MATRIX_DETERMINANT_REAL_HPP_INCLUDED
