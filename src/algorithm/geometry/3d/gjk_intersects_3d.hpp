#pragma once

#include "gjk_query_3d.hpp"

inline bool gjk_intersects_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L,
    std::size_t max_iterations = 96
){
    return gjk_query_3d(first, second, tolerance, max_iterations).intersects;
}
