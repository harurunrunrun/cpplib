#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_DIFFERENCE_3D_DIRECT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_DIFFERENCE_3D_DIRECT_HPP_INCLUDED

#include <algorithm>

#include "convex_polyhedron3.hpp"
#include "minkowski_sum_3d_direct.hpp"

inline ConvexPolyhedron3 minkowski_difference_3d_direct(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double general_position_margin = 1.0e-9L
){
    ConvexPolyhedron3 reflected = second;
    for(Point3& vertex: reflected.vertices) vertex = -vertex;
    for(auto& face: reflected.faces) std::swap(face[1], face[2]);
    return minkowski_sum_3d_direct(
        first, reflected, general_position_margin
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_DIFFERENCE_3D_DIRECT_HPP_INCLUDED
