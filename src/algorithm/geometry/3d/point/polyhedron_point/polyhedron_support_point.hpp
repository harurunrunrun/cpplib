#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYHEDRON_POINT_POLYHEDRON_SUPPORT_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYHEDRON_POINT_POLYHEDRON_SUPPORT_POINT_HPP_INCLUDED

#include "../../index/polyhedron_point/polyhedron_support_vertex.hpp"

inline Point3 polyhedron_support_point(
    const Polyhedron3& polyhedron,
    const Point3& direction
){
    return polyhedron.vertices[polyhedron_support_vertex(polyhedron, direction)];
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYHEDRON_POINT_POLYHEDRON_SUPPORT_POINT_HPP_INCLUDED
