#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_GJK_INTERSECTS_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_GJK_INTERSECTS_3D_HPP_INCLUDED

#include "../query/gjk_query_3d.hpp"

inline bool gjk_intersects_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L,
    std::size_t max_iterations = 96
){
    return gjk_query_3d(first, second, tolerance, max_iterations).intersects;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_GJK_INTERSECTS_3D_HPP_INCLUDED
