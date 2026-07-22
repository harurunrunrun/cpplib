#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_GREAT_CIRCLE_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_GREAT_CIRCLE_DISTANCE_HPP_INCLUDED
#include <cmath>
#include <stdexcept>

#include "spherical_central_angle.hpp"

inline long double great_circle_distance(
    const Point3& first,
    const Point3& second,
    long double radius = 1.0L
){
    geometry3d_spherical_detail::validate_radius(radius);
    const long double result = radius * spherical_central_angle(first, second);
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("great-circle distance overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_GREAT_CIRCLE_DISTANCE_HPP_INCLUDED
