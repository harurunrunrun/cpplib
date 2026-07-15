#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>

#include "convex_collision_result3.hpp"
#include "epa_3d.hpp"
#include "gjk_query_3d.hpp"

inline ConvexCollisionResult3 convex_collision_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-10L,
    std::size_t gjk_max_iterations = 96,
    std::size_t epa_max_iterations = 128
){
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument(
            "collision tolerance must be finite and positive"
        );
    }
    const GJKResult3 gjk = gjk_query_3d(
        first, second, tolerance * 0.01L, gjk_max_iterations
    );
    ConvexCollisionResult3 result;
    result.intersects = gjk.intersects;
    result.distance = gjk.distance;
    result.normal = gjk.normal;
    result.point_on_first = gjk.point_on_first;
    result.point_on_second = gjk.point_on_second;
    result.gjk_iterations = gjk.iterations;
    result.converged = gjk.converged;
    if(!gjk.intersects) return result;

    const EPAResult3 epa = epa_3d(
        first, second, gjk, tolerance, epa_max_iterations
    );
    result.penetration_depth = epa.penetration_depth;
    result.normal = epa.normal;
    result.point_on_first = epa.point_on_first;
    result.point_on_second = epa.point_on_second;
    result.epa_iterations = epa.iterations;
    result.converged = gjk.converged && epa.converged;
    return result;
}
