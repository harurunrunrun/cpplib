#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_LATITUDE_LONGITUDE_FROM_POINT3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_LATITUDE_LONGITUDE_FROM_POINT3_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../type/definition/latitude_longitude3_result.hpp"
#include "../../scalar/point_point/spherical_central_angle.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_LATITUDE_LONGITUDE_FROM_POINT3_HPP_INCLUDED
