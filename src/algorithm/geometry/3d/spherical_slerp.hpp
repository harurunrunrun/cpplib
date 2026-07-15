#pragma once

#include <cmath>
#include <limits>
#include <stdexcept>

#include "spherical_central_angle.hpp"

inline Point3 spherical_slerp(
    const Point3& first,
    const Point3& second,
    long double parameter
){
    if(!std::isfinite(parameter) || parameter < 0.0L || parameter > 1.0L)[[unlikely]]{
        throw std::invalid_argument("spherical slerp parameter outside [0,1]");
    }
    const Point3 start = geometry3d_spherical_detail::unit_direction(
        first, "invalid first direction in spherical_slerp"
    );
    const Point3 finish = geometry3d_spherical_detail::unit_direction(
        second, "invalid second direction in spherical_slerp"
    );
    if(geometry3d_spherical_detail::antipodal(start, finish))[[unlikely]]{
        throw std::domain_error("antipodal spherical interpolation is not unique");
    }
    const long double angle = spherical_central_angle(start, finish);
    if(angle <= 32.0L * std::numeric_limits<long double>::epsilon()){
        return geometry3d_spherical_detail::unit_direction(
            (1.0L - parameter) * start + parameter * finish,
            "degenerate spherical interpolation"
        );
    }
    const long double sine = std::sin(angle);
    const Point3 result =
        (std::sin((1.0L - parameter) * angle) / sine) * start
        + (std::sin(parameter * angle) / sine) * finish;
    return geometry3d_spherical_detail::unit_direction(
        result, "spherical interpolation overflow"
    );
}
