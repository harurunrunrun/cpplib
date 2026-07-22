#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_HALF_PLANE_INTERSECTION_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_HALF_PLANE_INTERSECTION_AREA_HPP_INCLUDED

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_HALF_PLANE_INTERSECTION_AREA_HPP_INCLUDED
