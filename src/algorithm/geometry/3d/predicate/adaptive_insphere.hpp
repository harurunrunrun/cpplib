#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ADAPTIVE_INSPHERE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ADAPTIVE_INSPHERE_HPP_INCLUDED

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "adaptive_orient3d.hpp"
#include "insphere.hpp"

namespace geometry3d_adaptive_detail{

inline ExactDyadic determinant3(
    const std::array<std::array<ExactDyadic, 3>, 3>& matrix
){
    return add(
        subtract(
            multiply(matrix[0][0], subtract(
                multiply(matrix[1][1], matrix[2][2]),
                multiply(matrix[1][2], matrix[2][1])
            )),
            multiply(matrix[0][1], subtract(
                multiply(matrix[1][0], matrix[2][2]),
                multiply(matrix[1][2], matrix[2][0])
            ))
        ),
        multiply(matrix[0][2], subtract(
            multiply(matrix[1][0], matrix[2][1]),
            multiply(matrix[1][1], matrix[2][0])
        ))
    );
}

inline ExactDyadic determinant4(
    const std::array<std::array<ExactDyadic, 4>, 4>& matrix
){
    ExactDyadic result{};
    for(std::size_t column = 0; column < 4; ++column){
        std::array<std::array<ExactDyadic, 3>, 3> minor{};
        for(std::size_t row = 1; row < 4; ++row){
            std::size_t destination = 0;
            for(std::size_t source = 0; source < 4; ++source){
                if(source == column) continue;
                minor[row - 1][destination++] = matrix[row][source];
            }
        }
        ExactDyadic term = multiply(matrix[0][column], determinant3(minor));
        if(column % 2 != 0) term = negate(term);
        result = add(result, term);
    }
    return result;
}

inline int exact_insphere_raw_sign(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth,
    const Point3& query
){
    const std::array<Point3, 4> points{first, second, third, fourth};
    std::array<std::array<ExactDyadic, 4>, 4> matrix{};
    for(std::size_t row = 0; row < points.size(); ++row){
        ExactDyadic squared_norm{};
        const std::array<long double, 3> coordinate{
            points[row].x, points[row].y, points[row].z
        };
        const std::array<long double, 3> query_coordinate{
            query.x, query.y, query.z
        };
        for(std::size_t axis = 0; axis < 3; ++axis){
            const ExactDyadic difference = subtract(
                exact_dyadic(coordinate[axis]), exact_dyadic(query_coordinate[axis])
            );
            matrix[row][axis] = difference;
            squared_norm = add(squared_norm, multiply(difference, difference));
        }
        matrix[row][3] = squared_norm;
    }
    return sign(determinant4(matrix));
}

}  // namespace geometry3d_adaptive_detail

inline int adaptive_insphere(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth,
    const Point3& query
){
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second)
        || !geometry3d_is_finite(third) || !geometry3d_is_finite(fourth)
        || !geometry3d_is_finite(query))[[unlikely]]{
        throw std::invalid_argument("non-finite point in adaptive_insphere");
    }
    const int orientation = adaptive_orient3d(first, second, third, fourth);
    if(orientation == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron in adaptive_insphere");
    }
    try{
        const auto normalized = geometry3d_predicate_detail::normalized_insphere_raw(
            first, second, third, fourth, query
        );
        const long double inside_value = orientation > 0
            ? -normalized.raw_determinant : normalized.raw_determinant;
        const long double error_bound = 128.0L
            * std::numeric_limits<long double>::epsilon() * normalized.permanent;
        if(std::abs(inside_value) > error_bound){
            return inside_value > 0.0L ? 1 : -1;
        }
    }catch(const std::overflow_error&){
        // Exact dyadic arithmetic below handles the full finite long-double range.
    }
    const int raw_sign = geometry3d_adaptive_detail::exact_insphere_raw_sign(
        first, second, third, fourth, query
    );
    return orientation > 0 ? -raw_sign : raw_sign;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ADAPTIVE_INSPHERE_HPP_INCLUDED
