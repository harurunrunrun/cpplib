#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SCALAR_POINT3_FROM_LATITUDE_LONGITUDE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SCALAR_POINT3_FROM_LATITUDE_LONGITUDE_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../aggregate/all/is_finite.hpp"
#include "../../scalar/point_point/spherical_central_angle.hpp"

inline Point3 point3_from_latitude_longitude(
    long double latitude,
    long double longitude,
    long double radius = 1.0L
){
    if(!std::isfinite(latitude) || !std::isfinite(longitude))[[unlikely]]{
        throw std::invalid_argument("non-finite latitude or longitude");
    }
    geometry3d_spherical_detail::validate_radius(radius);
    if(latitude < -GEOMETRY3D_PI / 2.0L - GEOMETRY3D_EPS
        || latitude > GEOMETRY3D_PI / 2.0L + GEOMETRY3D_EPS)[[unlikely]]{
        throw std::invalid_argument("latitude outside [-pi/2,pi/2]");
    }
    latitude = std::clamp(
        latitude, -GEOMETRY3D_PI / 2.0L, GEOMETRY3D_PI / 2.0L
    );
    const long double horizontal = radius * std::cos(latitude);
    const Point3 result{
        horizontal * std::cos(longitude),
        horizontal * std::sin(longitude),
        radius * std::sin(latitude),
    };
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error("latitude-longitude conversion overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SCALAR_POINT3_FROM_LATITUDE_LONGITUDE_HPP_INCLUDED
