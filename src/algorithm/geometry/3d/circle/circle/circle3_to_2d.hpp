#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_CIRCLE_CIRCLE3_TO_2D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_CIRCLE_CIRCLE3_TO_2D_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../../../2d/aggregate/all/types.hpp"
#include "../../scalar/point/abs.hpp"
#include "../../aggregate/all/is_finite.hpp"
#include "../../point/point/unit.hpp"
#include "../../scalar/point_point/dot.hpp"
#include "../../integer/scalar/geometry3d_sign.hpp"
#include "../../aggregate/all/plane_coordinate_system.hpp"
#include "../../point/point/to_plane_coordinates.hpp"
#include "../../predicate/point_point/parallel.hpp"

inline Circle circle3_to_2d(
    const Circle3& circle,
    const PlaneCoordinateSystem3& system
){
    geometry3d_validate(circle);
    if(!geometry3d_is_finite(system.origin) ||
        !geometry3d_is_finite(system.first_axis) ||
        !geometry3d_is_finite(system.second_axis) ||
        !geometry3d_is_finite(system.normal))[[unlikely]]{
        throw std::invalid_argument("circle conversion requires a finite coordinate system");
    }
    const Point3 first_axis = unit(system.first_axis);
    const Point3 second_axis = unit(system.second_axis);
    const Point3 system_normal = unit(system.normal);
    if(!parallel(circle.normal, system_normal))[[unlikely]]{
        throw std::invalid_argument("circle plane and coordinate system differ");
    }
    const auto difference = geometry3d_normalized_difference(
        circle.center, system.origin, {circle.radius}
    );
    const long double scale = difference.scale;
    const Point3 offset = difference.value;
    const long double offset_length = std::hypot(
        offset.x, offset.y, offset.z
    );
    if(geometry3d_scaled_sign(
        dot(offset, system_normal), offset_length
    ) != 0)[[unlikely]]{
        throw std::invalid_argument("circle center is outside the coordinate plane");
    }
    const long double x = dot(offset, first_axis) * scale;
    const long double y = dot(offset, second_axis) * scale;
    if(!std::isfinite(x) || !std::isfinite(y))[[unlikely]]{
        throw std::overflow_error("2D circle center is not representable");
    }
    return {{x, y}, circle.radius};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_CIRCLE_CIRCLE3_TO_2D_HPP_INCLUDED
