#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>

#include "matrix3.hpp"
#include "quaternion3.hpp"
#include "relative_equal.hpp"

inline Quaternion3 quaternion3_from_matrix3(
    const Matrix3& matrix,
    long double tolerance = 1.0e-9L
){
    if(!std::isfinite(tolerance) || tolerance < 0.0L or tolerance >= 1.0L)[[unlikely]]{
        throw std::invalid_argument("invalid rotation matrix tolerance");
    }
    for(std::size_t row = 0; row < 3; ++row){
        for(std::size_t column = 0; column < 3; ++column){
            if(!std::isfinite(matrix[row][column]))[[unlikely]]{
                throw std::invalid_argument("non-finite rotation matrix");
            }
        }
    }
    for(std::size_t first = 0; first < 3; ++first){
        for(std::size_t second = first; second < 3; ++second){
            long double product = 0.0L;
            for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
                product += matrix[first][coordinate] * matrix[second][coordinate];
            }
            const long double expected = first == second ? 1.0L : 0.0L;
            if(!relative_equal(product, expected, tolerance, tolerance))[[unlikely]]{
                throw std::invalid_argument("matrix is not orthonormal");
            }
        }
    }
    if(!relative_equal(matrix.determinant(), 1.0L, tolerance, tolerance))[[unlikely]]{
        throw std::invalid_argument("matrix is not a proper rotation");
    }

    Quaternion3 result;
    const long double trace = matrix[0][0] + matrix[1][1] + matrix[2][2];
    if(trace > 0.0L){
        const long double scale = 2.0L * std::sqrt(std::max(0.0L, trace + 1.0L));
        if(!(scale > 0.0L) or !std::isfinite(scale))[[unlikely]]{
            throw std::invalid_argument("degenerate quaternion extraction scale");
        }
        result = {
            scale / 4.0L,
            (matrix[2][1] - matrix[1][2]) / scale,
            (matrix[0][2] - matrix[2][0]) / scale,
            (matrix[1][0] - matrix[0][1]) / scale,
        };
    }else if(matrix[0][0] >= matrix[1][1] && matrix[0][0] >= matrix[2][2]){
        const long double scale = 2.0L * std::sqrt(std::max(
            0.0L, 1.0L + matrix[0][0] - matrix[1][1] - matrix[2][2]
        ));
        if(!(scale > 0.0L) or !std::isfinite(scale))[[unlikely]]{
            throw std::invalid_argument("degenerate quaternion extraction scale");
        }
        result = {
            (matrix[2][1] - matrix[1][2]) / scale,
            scale / 4.0L,
            (matrix[0][1] + matrix[1][0]) / scale,
            (matrix[0][2] + matrix[2][0]) / scale,
        };
    }else if(matrix[1][1] >= matrix[2][2]){
        const long double scale = 2.0L * std::sqrt(std::max(
            0.0L, 1.0L + matrix[1][1] - matrix[0][0] - matrix[2][2]
        ));
        if(!(scale > 0.0L) or !std::isfinite(scale))[[unlikely]]{
            throw std::invalid_argument("degenerate quaternion extraction scale");
        }
        result = {
            (matrix[0][2] - matrix[2][0]) / scale,
            (matrix[0][1] + matrix[1][0]) / scale,
            scale / 4.0L,
            (matrix[1][2] + matrix[2][1]) / scale,
        };
    }else{
        const long double scale = 2.0L * std::sqrt(std::max(
            0.0L, 1.0L + matrix[2][2] - matrix[0][0] - matrix[1][1]
        ));
        if(!(scale > 0.0L) or !std::isfinite(scale))[[unlikely]]{
            throw std::invalid_argument("degenerate quaternion extraction scale");
        }
        result = {
            (matrix[1][0] - matrix[0][1]) / scale,
            (matrix[0][2] + matrix[2][0]) / scale,
            (matrix[1][2] + matrix[2][1]) / scale,
            scale / 4.0L,
        };
    }
    result = result.normalized();
    if(result.w < 0.0L || (result.w == 0.0L && (
        result.x < 0.0L || (result.x == 0.0L && (
            result.y < 0.0L || (result.y == 0.0L && result.z < 0.0L)
        ))
    ))) result = -result;
    return result;
}
