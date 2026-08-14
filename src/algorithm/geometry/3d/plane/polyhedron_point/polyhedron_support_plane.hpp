#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_POLYHEDRON_POINT_POLYHEDRON_SUPPORT_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_POLYHEDRON_POINT_POLYHEDRON_SUPPORT_PLANE_HPP_INCLUDED

#include "../../point/polyhedron_point/polyhedron_support_point.hpp"
#include "../../type/definition/plane3.hpp"

inline Plane3 polyhedron_support_plane(
    const Polyhedron3& polyhedron,
    const Point3& outward_normal
){
    return {polyhedron_support_point(polyhedron, outward_normal), outward_normal};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_POLYHEDRON_POINT_POLYHEDRON_SUPPORT_PLANE_HPP_INCLUDED
