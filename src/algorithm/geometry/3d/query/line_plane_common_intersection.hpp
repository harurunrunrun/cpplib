#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_LINE_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_LINE_PLANE_COMMON_INTERSECTION_HPP_INCLUDED

#include "../predicate/is_finite.hpp"
#include "../point/line_plane_intersection.hpp"
#include "linear_intersection3.hpp"
#include "../predicate/on_plane.hpp"

inline LinearIntersection3 line_plane_common_intersection(
    const Line3& line,
    const Plane3& plane
){
    if(!geometry3d_is_finite(line) || !geometry3d_is_finite(plane))[[unlikely]]{
        throw std::invalid_argument(
            "line_plane_common_intersection requires finite inputs"
        );
    }
    const auto intersection = geometry3d_line_plane_detail::intersection_data(
        line.a, line.b, plane
    );
    if(intersection) return intersection->point;
    if(on_plane(plane, line.a)) return line;
    return std::monostate{};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_LINE_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
