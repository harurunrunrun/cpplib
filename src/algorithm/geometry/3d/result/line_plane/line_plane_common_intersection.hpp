#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_LINE_PLANE_LINE_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_LINE_PLANE_LINE_PLANE_COMMON_INTERSECTION_HPP_INCLUDED

#include "../../aggregate/all/is_finite.hpp"
#include "../../point/line_plane/line_plane_intersection.hpp"
#include "../../type/definition/linear_intersection3.hpp"
#include "../../predicate/plane_point/on_plane.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_LINE_PLANE_LINE_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
