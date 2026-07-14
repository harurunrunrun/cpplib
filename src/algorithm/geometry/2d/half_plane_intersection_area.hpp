#pragma once

#include <cmath>
#include <limits>
#include <vector>

#include "advanced/half_plane_intersection.hpp"
#include "area.hpp"

inline long double half_plane_intersection_area(
    const std::vector<Line>& half_planes
){
    const HalfPlaneIntersectionResult result =
        half_plane_intersection(half_planes);
    if(result.kind == HALF_PLANE_EMPTY) return 0.0L;
    if(result.kind == HALF_PLANE_UNBOUNDED){
        return std::numeric_limits<long double>::infinity();
    }
    return std::abs(area(result.polygon));
}
