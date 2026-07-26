#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_EPA_3D_SHAPES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_EPA_3D_SHAPES_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <stdexcept>

#include "epa_3d_detail.hpp"

inline EPAResult3 epa_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-10L,
    std::size_t max_iterations = 128
){
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument("EPA tolerance must be finite and positive");
    }
    if(max_iterations == 0)[[unlikely]]{
        throw std::invalid_argument("EPA iteration limit must be positive");
    }
    epa_3d_detail::validate_full_dimensional(first, second);
    const gjk_3d_detail::CollisionNormalization3 normalization =
        gjk_3d_detail::normalize_pair(first, second);
    const GJKResult3 gjk_result = gjk_3d_detail::gjk_query_core(
        normalization.first, normalization.second,
        tolerance * 0.01L, 96
    );
    return epa_3d_detail::restore_result(
        epa_3d_detail::epa_core(
            normalization.first, normalization.second,
            gjk_result, tolerance, max_iterations
        ),
        normalization
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_EPA_3D_SHAPES_HPP_INCLUDED
