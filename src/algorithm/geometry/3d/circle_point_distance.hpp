#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_POINT_DISTANCE_HPP_INCLUDED

#include <cmath>
#include <algorithm>
#include <limits>
#include <stdexcept>

#include "abs.hpp"
#include "base.hpp"
#include "is_finite.hpp"
#include "dot.hpp"
#include "plane3_unit_normal.hpp"

inline long double distance(const Circle3& circle, const Point3& point){
    geometry3d_validate(circle);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("circle-point distance requires a finite point");
    }
    const Point3 normal = plane3_unit_normal({circle.center, circle.normal});
    const auto difference = geometry3d_normalized_difference(
        point, circle.center, {circle.radius}
    );
    const Point3 offset = difference.value;
    const long double height = dot(offset, normal);
    const Point3 radial = offset - normal * height;
    const long double radial_distance = std::hypot(
        radial.x, radial.y, radial.z
    );
    const long double normalized_distance = std::hypot(
        height, radial_distance - circle.radius / difference.scale
    );
    if(normalized_distance > std::numeric_limits<long double>::max() / difference.scale)
        [[unlikely]]{
        throw std::overflow_error(
            "circle-point distance is not representable"
        );
    }
    return normalized_distance * difference.scale;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_POINT_DISTANCE_HPP_INCLUDED
