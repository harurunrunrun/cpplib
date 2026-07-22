#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GJK_DISTANCE_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GJK_DISTANCE_3D_HPP_INCLUDED

#include "gjk_query_3d.hpp"

inline long double gjk_distance_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L,
    std::size_t max_iterations = 96
){
    return gjk_query_3d(first, second, tolerance, max_iterations).distance;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GJK_DISTANCE_3D_HPP_INCLUDED
