#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LATITUDE_LONGITUDE3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LATITUDE_LONGITUDE3_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "spherical_central_angle.hpp"

struct LatitudeLongitude3{
    long double latitude = 0.0L;
    long double longitude = 0.0L;
    long double radius = 0.0L;
};

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

inline LatitudeLongitude3 latitude_longitude_from_point3(const Point3& point){
    const Point3 direction = geometry3d_spherical_detail::unit_direction(
        point, "invalid point in latitude_longitude_from_point3"
    );
    const long double radius = std::hypot(point.x, point.y, point.z);
    if(!std::isfinite(radius))[[unlikely]]{
        throw std::overflow_error("point radius overflow in latitude-longitude conversion");
    }
    const long double horizontal = std::hypot(direction.x, direction.y);
    const long double longitude = horizontal <= 64.0L
        * std::numeric_limits<long double>::epsilon()
        ? 0.0L : std::atan2(direction.y, direction.x);
    return {
        std::asin(std::clamp(direction.z, -1.0L, 1.0L)),
        longitude,
        radius,
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LATITUDE_LONGITUDE3_HPP_INCLUDED
