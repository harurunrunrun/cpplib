#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_SUM_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_SUM_3D_DEFAULT_HPP_INCLUDED

#include "convex_hull_3d_default.hpp"
#include "minkowski_sum_3d_detail.hpp"

inline ConvexPolyhedron3 minkowski_sum_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    return convex_hull_3d(
        minkowski_sum_3d_detail::candidate_points(first, second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_SUM_3D_DEFAULT_HPP_INCLUDED
