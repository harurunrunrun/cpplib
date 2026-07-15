#pragma once

#include <optional>

#include "base.hpp"
#include "is_finite.hpp"
#include "line_plane_intersection.hpp"
#include "on_plane.hpp"

inline std::optional<Point3> segment_plane_intersection(
    const Segment3& segment,
    const Plane3& plane
){
    if(!geometry3d_is_finite(segment) || !geometry3d_is_finite(plane))
        [[unlikely]]{
        throw std::invalid_argument(
            "segment-plane intersection requires finite inputs"
        );
    }
    if(geometry3d_line_plane_detail::exact_same_point(
        segment.a, segment.b
    )){
        return on_plane(plane, segment.a)
            ? std::optional<Point3>(segment.a) : std::nullopt;
    }
    const auto intersection = geometry3d_line_plane_detail::intersection_data(
        segment.a, segment.b, plane
    );
    if(!intersection
        || geometry3d_line_plane_detail::parameter_sign(*intersection) < 0
        || geometry3d_line_plane_detail::parameter_end_sign(*intersection) > 0){
        return std::nullopt;
    }
    return intersection->point;
}
