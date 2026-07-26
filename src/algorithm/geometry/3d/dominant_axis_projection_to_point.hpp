#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DOMINANT_AXIS_PROJECTION_TO_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DOMINANT_AXIS_PROJECTION_TO_POINT_HPP_INCLUDED

#include "../2d/types.hpp"
#include "dominant_axis3.hpp"
#include "geometry_primitives.hpp"

inline Point dominant_axis_projection(
    const Point3& point,
    DominantAxis3 axis
){
    if(axis == DominantAxis3::x) return {point.y, point.z};
    if(axis == DominantAxis3::y) return {point.z, point.x};
    return {point.x, point.y};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DOMINANT_AXIS_PROJECTION_TO_POINT_HPP_INCLUDED
