#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TETRAHEDRON_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TETRAHEDRON_POINT_DISTANCE_HPP_INCLUDED

#include "abs.hpp"
#include "../core/geometry_primitives.hpp"
#include "../point/tetrahedron_closest_point.hpp"

inline long double distance(
    const Tetrahedron3& tetrahedron,
    const Point3& point
){
    return abs(point - closest_point(tetrahedron, point));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TETRAHEDRON_POINT_DISTANCE_HPP_INCLUDED
