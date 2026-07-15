#pragma once

#include <algorithm>
#include <cmath>

#include "../../src/algorithm/geometry/3d/convex_hull_3d.hpp"

inline ConvexPolyhedron3 geometry3d_test_cube(Point3 shift = {}){
    std::vector<Point3> points;
    for(int x: {-1, 1}) for(int y: {-1, 1}) for(int z: {-1, 1}){
        points.push_back({shift.x + x, shift.y + y, shift.z + z});
    }
    return convex_hull_3d(std::move(points));
}

inline bool geometry3d_convex_close(
    long double first,
    long double second,
    long double tolerance = 1.0e-9L
){
    return std::abs(first - second) <= tolerance
        * std::max({1.0L, std::abs(first), std::abs(second)});
}

inline bool geometry3d_convex_close(
    const Point3& first,
    const Point3& second,
    long double tolerance = 1.0e-9L
){
    return geometry3d_convex_close(first.x, second.x, tolerance)
        && geometry3d_convex_close(first.y, second.y, tolerance)
        && geometry3d_convex_close(first.z, second.z, tolerance);
}
