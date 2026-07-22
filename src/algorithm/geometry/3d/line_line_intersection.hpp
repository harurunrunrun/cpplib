#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_INTERSECTION_HPP_INCLUDED

#include <variant>

#include "abs.hpp"
#include "base.hpp"
#include "closest_points.hpp"
#include "geometry3d_sign.hpp"
#include "linear_intersection3.hpp"
#include "line3_direction.hpp"
#include "on_line.hpp"
#include "parallel.hpp"

inline LinearIntersection3 line_line_intersection(
    const Line3& first,
    const Line3& second
){
    const Point3 first_direction = line3_direction(first);
    const Point3 second_direction = line3_direction(second);
    if(parallel(first_direction, second_direction)){
        if(on_line(first, second.a)) return first;
        return std::monostate{};
    }
    const auto [on_first, on_second] = closest_points(first, second);
    if(geometry3d_sign(abs(on_first - on_second)) != 0){
        return std::monostate{};
    }
    return (on_first + on_second) / 2;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_INTERSECTION_HPP_INCLUDED
