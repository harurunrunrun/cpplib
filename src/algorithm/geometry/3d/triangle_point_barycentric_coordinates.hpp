#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_POINT_BARYCENTRIC_COORDINATES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_POINT_BARYCENTRIC_COORDINATES_HPP_INCLUDED

#include <array>
#include <cmath>
#include <stdexcept>

#include "barycentric_coordinates_detail.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "point3.hpp"
#include "triangle3.hpp"

inline std::array<long double, 3> barycentric_coordinates(
    const Triangle3& triangle,
    const Point3& point
){
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument(
            "barycentric coordinates require finite points"
        );
    }
    const std::array<Point3, 3> vertices{
        triangle.a, triangle.b, triangle.c,
    };
    const long double edge_scale =
        geometry3d_barycentric_detail::edge_scale(vertices);
    if(edge_scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    Point3 first = geometry3d_barycentric_detail::normalized_difference(
        triangle.b, triangle.a, edge_scale
    );
    Point3 second = geometry3d_barycentric_detail::normalized_difference(
        triangle.c, triangle.a, edge_scale
    );
    Point3 offset = geometry3d_barycentric_detail::normalized_difference(
        point, triangle.a, edge_scale
    );
    const long double scale =
        geometry3d_barycentric_detail::vector_scale(first, second);
    if(scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    first /= scale;
    second /= scale;
    offset /= scale;
    if(!geometry3d_is_finite(offset))[[unlikely]]{
        throw std::overflow_error("barycentric triangle weights overflow");
    }
    const Point3 normal = cross(first, second);
    const long double denominator = dot(normal, normal);
    if(denominator == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const long double second_weight =
        dot(cross(offset, second), normal) / denominator;
    const long double third_weight =
        dot(cross(first, offset), normal) / denominator;
    const long double first_weight = 1 - second_weight - third_weight;
    if(!std::isfinite(first_weight) || !std::isfinite(second_weight)
        || !std::isfinite(third_weight))[[unlikely]]{
        throw std::overflow_error("barycentric triangle weights overflow");
    }
    return {first_weight, second_weight, third_weight};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_POINT_BARYCENTRIC_COORDINATES_HPP_INCLUDED
