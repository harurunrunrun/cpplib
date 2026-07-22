#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_TETRAHEDRON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_TETRAHEDRON_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <limits>

#include "barycentric_coordinates.hpp"
#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "line3_direction.hpp"
#include "linear_intersection3.hpp"
#include "tetrahedron_orientation.hpp"

inline LinearIntersection3 line_tetrahedron_intersection(
    const Line3& line,
    const Tetrahedron3& tetrahedron
){
    const Point3 direction = line3_direction(line);
    if(tetrahedron_orientation(tetrahedron) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const auto at_zero = barycentric_coordinates(tetrahedron, line.a);
    const auto at_one = barycentric_coordinates(tetrahedron, line.b);
    long double lower = -std::numeric_limits<long double>::infinity();
    long double upper = std::numeric_limits<long double>::infinity();
    for(std::size_t index = 0; index < 4; ++index){
        const long double slope = at_one[index] - at_zero[index];
        const int slope_sign = geometry3d_sign(slope);
        if(slope_sign == 0){
            if(geometry3d_sign(at_zero[index]) < 0) return std::monostate{};
            continue;
        }
        const long double boundary = -at_zero[index] / slope;
        if(slope_sign > 0) lower = std::max(lower, boundary);
        else upper = std::min(upper, boundary);
    }
    const int interval_sign = geometry3d_sign(upper - lower);
    if(interval_sign < 0) return std::monostate{};
    if(interval_sign == 0) return line.a + direction * ((lower + upper) / 2);
    return Segment3{
        line.a + direction * lower,
        line.a + direction * upper,
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_TETRAHEDRON_INTERSECTION_HPP_INCLUDED
