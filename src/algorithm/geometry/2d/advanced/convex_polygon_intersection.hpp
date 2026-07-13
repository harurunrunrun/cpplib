#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "half_plane_intersection.hpp"

inline std::vector<Point> convex_polygon_intersection(
    std::vector<Point> first,
    std::vector<Point> second
){
    using namespace advanced_geometry_detail;

    first = normalize_convex_polygon(std::move(first));
    second = normalize_convex_polygon(std::move(second));
    if(first.empty() || second.empty()) return {};
    if(first.size() == 1){
        return point_in_normalized_convex_polygon(second, first.front())
            ? first
            : std::vector<Point>{};
    }
    if(second.size() == 1){
        return point_in_normalized_convex_polygon(first, second.front())
            ? second
            : std::vector<Point>{};
    }
    if(first.size() == 2 && second.size() == 2){
        return segment_intersection_set(
            {first[0], first[1]},
            {second[0], second[1]}
        );
    }
    if(first.size() == 2){
        return clip_segment_by_convex_polygon({first[0], first[1]}, second);
    }
    if(second.size() == 2){
        return clip_segment_by_convex_polygon({second[0], second[1]}, first);
    }

    std::vector<Line> half_planes;
    half_planes.reserve(first.size() + second.size());
    for(std::size_t index = 0; index < first.size(); ++index){
        half_planes.push_back({first[index], first[(index + 1) % first.size()]});
    }
    for(std::size_t index = 0; index < second.size(); ++index){
        half_planes.push_back({second[index], second[(index + 1) % second.size()]});
    }
    HalfPlaneIntersectionResult result = half_plane_intersection(std::move(half_planes));
    if(result.kind == HALF_PLANE_EMPTY) return {};
    if(result.kind != HALF_PLANE_BOUNDED){
        throw std::runtime_error("intersection of two bounded convex polygons became unbounded");
    }
    return normalize_convex_polygon(std::move(result.polygon));
}
