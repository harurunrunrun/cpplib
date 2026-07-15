#pragma once

#include <utility>

#include "gjk_query_3d.hpp"

inline std::pair<Point3, Point3> gjk_closest_points_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L,
    std::size_t max_iterations = 96
){
    const GJKResult3 result =
        gjk_query_3d(first, second, tolerance, max_iterations);
    return {result.point_on_first, result.point_on_second};
}
