#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TO_PLANE_COORDINATES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TO_PLANE_COORDINATES_HPP_INCLUDED

#include "../../2d/core/types.hpp"
#include "../scalar/dot.hpp"
#include "../core/plane_coordinate_system.hpp"

inline Point to_plane_coordinates(
    const PlaneCoordinateSystem3& system,
    const Point3& point
){
    const Point3 offset = point - system.origin;
    return {dot(offset, system.first_axis), dot(offset, system.second_axis)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TO_PLANE_COORDINATES_HPP_INCLUDED
