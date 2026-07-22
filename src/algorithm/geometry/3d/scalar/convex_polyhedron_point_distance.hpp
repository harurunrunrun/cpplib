#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_CONVEX_POLYHEDRON_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_CONVEX_POLYHEDRON_POINT_DISTANCE_HPP_INCLUDED

#include "../point/convex_polyhedron_closest_point.hpp"
#include "distance.hpp"

inline long double convex_polyhedron_point_distance(
    const ConvexPolyhedron3& polyhedron,
    const Point3& point
){
    return distance(convex_polyhedron_closest_point(polyhedron, point), point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_CONVEX_POLYHEDRON_POINT_DISTANCE_HPP_INCLUDED
