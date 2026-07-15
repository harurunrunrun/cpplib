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
    const gjk_3d_detail::CollisionNormalization3 normalization =
        gjk_3d_detail::normalize_pair(first, second);
    const GJKResult3 gjk = gjk_3d_detail::gjk_query_core(
        normalization.first, normalization.second,
        tolerance * 0.01L, gjk_max_iterations
    );
    ConvexCollisionResult3 result;
    result.intersects = gjk.intersects;
    result.gjk_iterations = gjk.iterations;
    result.converged = gjk.converged;
    if(!gjk.intersects){
        result.distance = gjk_3d_detail::restore_length(
            gjk.distance, normalization
        );
        result.normal = gjk_3d_detail::finite_unit_normal(gjk.normal);
        result.point_on_first = gjk_3d_detail::restore_point(
            gjk.point_on_first, normalization
        );
        result.point_on_second = gjk_3d_detail::restore_point(
            gjk.point_on_second, normalization
        );
        return result;
    }
    if(first.affine_dimension != 3 || second.affine_dimension != 3){
        result.penetration_depth = 0.0L;
        result.normal = {};
        result.point_on_first = gjk_3d_detail::restore_point(
            gjk.point_on_first, normalization
        );
        result.point_on_second = gjk_3d_detail::restore_point(
            gjk.point_on_second, normalization
        );
        result.epa_iterations = 0;
        return result;
    }

    const EPAResult3 epa = epa_3d_detail::restore_result(
        epa_3d_detail::epa_core(
            normalization.first, normalization.second,
            gjk, tolerance, epa_max_iterations
        ),
        normalization
    );
    result.penetration_depth = epa.penetration_depth;
    if(epa.intersects){
        result.normal = epa.normal;
        result.point_on_first = epa.point_on_first;
        result.point_on_second = epa.point_on_second;
    }
    result.epa_iterations = epa.iterations;
    result.converged = gjk.converged && epa.converged;
    return result;
}
