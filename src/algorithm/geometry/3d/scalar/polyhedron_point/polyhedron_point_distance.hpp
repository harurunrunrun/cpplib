#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POINT_POLYHEDRON_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POINT_POLYHEDRON_POINT_DISTANCE_HPP_INCLUDED

#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../point/polyhedron_point/polyhedron_closest_point.hpp"

inline long double polyhedron_point_distance(
    const Polyhedron3& polyhedron,
    const Point3& point
){
    return geometry3d_linear_closest_detail::checked_distance(
        point, polyhedron_closest_point(polyhedron, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POINT_POLYHEDRON_POINT_DISTANCE_HPP_INCLUDED
