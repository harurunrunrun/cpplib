#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INSPHERE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INSPHERE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "is_finite.hpp"
#include "orient3d.hpp"
#include "scale_aware_sign.hpp"

namespace geometry3d_predicate_detail{

inline long double determinant3(
    long double a00, long double a01, long double a02,
    long double a10, long double a11, long double a12,
    long double a20, long double a21, long double a22
){
    return a00 * (a11 * a22 - a12 * a21)
        - a01 * (a10 * a22 - a12 * a20)
        + a02 * (a10 * a21 - a11 * a20);
}

inline long double determinant4(
    const std::array<std::array<long double, 4>, 4>& matrix
){
    return matrix[0][0] * determinant3(
            matrix[1][1], matrix[1][2], matrix[1][3],
            matrix[2][1], matrix[2][2], matrix[2][3],
            matrix[3][1], matrix[3][2], matrix[3][3]
        )
        - matrix[0][1] * determinant3(
            matrix[1][0], matrix[1][2], matrix[1][3],
            matrix[2][0], matrix[2][2], matrix[2][3],
            matrix[3][0], matrix[3][2], matrix[3][3]
        )
        + matrix[0][2] * determinant3(
            matrix[1][0], matrix[1][1], matrix[1][3],
            matrix[2][0], matrix[2][1], matrix[2][3],
            matrix[3][0], matrix[3][1], matrix[3][3]
        )
        - matrix[0][3] * determinant3(
            matrix[1][0], matrix[1][1], matrix[1][2],
            matrix[2][0], matrix[2][1], matrix[2][2],
            matrix[3][0], matrix[3][1], matrix[3][2]
        );
}

inline long double determinant4_permanent(
    const std::array<std::array<long double, 4>, 4>& matrix
){
    std::array<int, 4> permutation{0, 1, 2, 3};
    long double result = 0.0L;
    do{
        long double term = 1.0L;
        for(int row = 0; row < 4; ++row){
            term *= std::abs(matrix[static_cast<std::size_t>(row)][
                static_cast<std::size_t>(permutation[static_cast<std::size_t>(row)])
            ]);
        }
        result += term;
    }while(std::next_permutation(permutation.begin(), permutation.end()));
    return result;
}

struct NormalizedInsphere{
    long double raw_determinant = 0.0L;
    long double scale = 0.0L;
    long double permanent = 0.0L;
};

inline NormalizedInsphere normalized_insphere_raw(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth,
    const Point3& query
){
    std::array<Point3, 4> difference{
        first - query, second - query, third - query, fourth - query
    };
    for(const Point3& value: difference) if(!geometry3d_is_finite(value))[[unlikely]]{
        throw std::overflow_error("insphere coordinate difference overflow");
    }
    long double scale = 0.0L;
    for(const Point3& value: difference){
        scale = std::max({
            scale, std::abs(value.x), std::abs(value.y), std::abs(value.z)
        });
    }
    if(scale == 0.0L) return {};
    std::array<std::array<long double, 4>, 4> matrix{};
    for(std::size_t row = 0; row < difference.size(); ++row){
        const Point3 value = difference[row] / scale;
        matrix[row] = {
            value.x,
            value.y,
            value.z,
            value.x * value.x + value.y * value.y + value.z * value.z,
        };
    }
    return {determinant4(matrix), scale, determinant4_permanent(matrix)};
}

}  // namespace geometry3d_predicate_detail

inline long double insphere(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth,
    const Point3& query
){
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second)
        || !geometry3d_is_finite(third) || !geometry3d_is_finite(fourth)
        || !geometry3d_is_finite(query))[[unlikely]]{
        throw std::invalid_argument("non-finite point in insphere");
    }
    const auto orientation = geometry3d_predicate_detail::normalized_orient3d(
        first, second, third, fourth
    );
    if(scale_aware_sign(orientation.determinant, orientation.permanent) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron in insphere");
    }
    const auto raw = geometry3d_predicate_detail::normalized_insphere_raw(
        first, second, third, fourth, query
    );
    const long double normalized_value = orientation.determinant > 0.0L
        ? -raw.raw_determinant : raw.raw_determinant;
    return geometry3d_predicate_detail::restore_homogeneous_scale(
        normalized_value, raw.scale, 5
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INSPHERE_HPP_INCLUDED
