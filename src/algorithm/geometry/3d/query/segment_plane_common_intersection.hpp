#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_SEGMENT_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_SEGMENT_PLANE_COMMON_INTERSECTION_HPP_INCLUDED

#include "../predicate/is_finite.hpp"
#include "../point/line_plane_intersection.hpp"
#include "linear_intersection3.hpp"
#include "../predicate/on_plane.hpp"

inline LinearIntersection3 segment_plane_common_intersection(
    const Segment3& segment,
    const Plane3& plane
){
    if(!geometry3d_is_finite(segment) || !geometry3d_is_finite(plane))
        [[unlikely]]{
        throw std::invalid_argument(
            "segment_plane_common_intersection requires finite inputs"
        );
    }
    if(geometry3d_line_plane_detail::exact_same_point(
        segment.a, segment.b
    )){
        if(on_plane(plane, segment.a)) return segment.a;
        return std::monostate{};
    }
    const auto intersection = geometry3d_line_plane_detail::intersection_data(
        segment.a, segment.b, plane
    );
    if(!intersection){
        if(on_plane(plane, segment.a)) return segment;
        return std::monostate{};
    }
    if(geometry3d_line_plane_detail::parameter_sign(*intersection) < 0
        || geometry3d_line_plane_detail::parameter_end_sign(*intersection) > 0){
        return std::monostate{};
    }
    return intersection->point;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_SEGMENT_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
