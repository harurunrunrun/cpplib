#pragma once

#include <optional>

#include "convex_polyhedron_contains.hpp"
#include "convex_polyhedron_line_interval.hpp"

inline std::optional<Segment3> convex_polyhedron_segment_intersection(
    const ConvexPolyhedron3& polyhedron,
    const Segment3& segment
){
    if(segment.a == segment.b){
        if(convex_polyhedron_contains(polyhedron, segment.a)) return segment;
        return std::nullopt;
    }
    const auto interval = convex_polyhedron_interval_detail::clip(
        polyhedron, segment.a, segment.b, 0.0L, 1.0L
    );
    if(!interval) return std::nullopt;
    return Segment3{
        segment.a * (1.0L - interval->first) + segment.b * interval->first,
        segment.a * (1.0L - interval->second) + segment.b * interval->second,
    };
}
