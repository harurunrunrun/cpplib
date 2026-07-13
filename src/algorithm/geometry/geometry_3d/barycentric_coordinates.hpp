#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "scalar_triple.hpp"

inline std::array<long double, 3> barycentric_coordinates(
    const Triangle3& triangle,
    const Point3& point
){
    const Point3 first = triangle.b - triangle.a;
    const Point3 second = triangle.c - triangle.a;
    const Point3 offset = point - triangle.a;
    const long double d00 = dot(first, first);
    const long double d01 = dot(first, second);
    const long double d11 = dot(second, second);
    const long double d20 = dot(offset, first);
    const long double d21 = dot(offset, second);
    const long double denominator = d00 * d11 - d01 * d01;
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const long double second_weight = (d11 * d20 - d01 * d21) / denominator;
    const long double third_weight = (d00 * d21 - d01 * d20) / denominator;
    return {1 - second_weight - third_weight, second_weight, third_weight};
}

inline std::array<long double, 4> barycentric_coordinates(
    const Tetrahedron3& tetrahedron,
    const Point3& point
){
    const Point3 first = tetrahedron.b - tetrahedron.a;
    const Point3 second = tetrahedron.c - tetrahedron.a;
    const Point3 third = tetrahedron.d - tetrahedron.a;
    const Point3 offset = point - tetrahedron.a;
    const long double denominator = scalar_triple(first, second, third);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const long double second_weight =
        scalar_triple(offset, second, third) / denominator;
    const long double third_weight =
        scalar_triple(first, offset, third) / denominator;
    const long double fourth_weight =
        scalar_triple(first, second, offset) / denominator;
    return {
        1 - second_weight - third_weight - fourth_weight,
        second_weight,
        third_weight,
        fourth_weight,
    };
}
