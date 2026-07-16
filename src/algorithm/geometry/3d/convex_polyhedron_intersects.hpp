#pragma once

#include <cstddef>

#include "gjk_intersects_3d.hpp"

inline bool convex_polyhedron_intersects(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L,
    std::size_t max_iterations = 96
){
    return gjk_intersects_3d(
        first, second, tolerance, max_iterations
    );
}
