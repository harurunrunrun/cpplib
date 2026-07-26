#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_FROM_PLANE_COORDINATES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_FROM_PLANE_COORDINATES_HPP_INCLUDED

#include "../2d/types.hpp"
#include "plane_coordinate_system.hpp"

inline Point3 from_plane_coordinates(
    const PlaneCoordinateSystem3& system,
    const Point& point
){
    return system.origin
        + system.first_axis * point.x
        + system.second_axis * point.y;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_FROM_PLANE_COORDINATES_HPP_INCLUDED
