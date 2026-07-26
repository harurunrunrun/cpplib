#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SUPPORT_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SUPPORT_PLANE_HPP_INCLUDED

#include "convex_polyhedron_support_point.hpp"

inline Plane3 convex_polyhedron_support_plane(
    const ConvexPolyhedron3& polyhedron,
    const Point3& outward_normal
){
    return {
        convex_polyhedron_support_point(polyhedron, outward_normal),
        outward_normal,
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SUPPORT_PLANE_HPP_INCLUDED
