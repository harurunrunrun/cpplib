#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_SUM_3D_DIRECT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_SUM_3D_DIRECT_HPP_INCLUDED

#include "minkowski_sum_3d_detail.hpp"

inline ConvexPolyhedron3 minkowski_sum_3d_direct(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double general_position_margin = 1.0e-9L
){
    return minkowski_sum_3d_detail::direct_sum(
        first, second, general_position_margin
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_SUM_3D_DIRECT_HPP_INCLUDED
