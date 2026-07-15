#pragma once

#include <optional>
#include <utility>

#include "cross.hpp"
#include "spherical_central_angle.hpp"

inline std::optional<std::pair<Point3, Point3>> great_circle_intersections(
    const Point3& first_normal,
    const Point3& second_normal
){
    const Point3 first = geometry3d_spherical_detail::unit_direction(
        first_normal, "invalid first great-circle normal"
    );
    const Point3 second = geometry3d_spherical_detail::unit_direction(
        second_normal, "invalid second great-circle normal"
    );
    const Point3 direction = cross(first, second);
    const long double length = std::hypot(direction.x, direction.y, direction.z);
    if(length <= 64.0L * std::numeric_limits<long double>::epsilon()){
        return std::nullopt;
    }
    const Point3 point = direction / length;
    return std::pair<Point3, Point3>{point, -point};
}
