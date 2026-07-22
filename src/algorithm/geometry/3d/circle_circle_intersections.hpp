#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_CIRCLE_INTERSECTIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_CIRCLE_INTERSECTIONS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "abs.hpp"
#include "base.hpp"
#include "is_finite.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "line_circle_intersections.hpp"
#include "on_circle.hpp"
#include "on_plane.hpp"
#include "parallel.hpp"
#include "plane3_unit_normal.hpp"
#include "plane_plane_intersection.hpp"

inline std::vector<Point3> circle_circle_intersections(
    const Circle3& first,
    const Circle3& second
){
    geometry3d_validate(first);
    geometry3d_validate(second);
    const Plane3 first_plane{first.center, first.normal};
    const Plane3 second_plane{second.center, second.normal};
    const Point3 first_normal = plane3_unit_normal(first_plane);
    const Point3 second_normal = plane3_unit_normal(second_plane);

    if(!parallel(first_normal, second_normal)){
        std::vector<Point3> result;
        for(const Point3& point: line_circle_intersections(
            plane_plane_intersection(first_plane, second_plane), first
        )){
            if(on_circle(second, point)) result.push_back(point);
        }
        return result;
    }
    const auto center_difference = geometry3d_normalized_difference(
        second.center, first.center, {first.radius, second.radius}
    );
    const long double scale = center_difference.scale;
    const long double first_radius = first.radius / scale;
    const long double second_radius = second.radius / scale;
    const Point3 difference = center_difference.value;
    const long double center_distance = std::hypot(
        difference.x, difference.y, difference.z
    );
    const long double linear_scale = std::max({
        center_distance, first_radius, second_radius,
    });
    if(geometry3d_scaled_sign(
        dot(first_normal, difference), center_distance
    ) != 0) return {};
    if(geometry3d_scaled_sign(center_distance, linear_scale) == 0){
        if(geometry3d_scaled_sign(
            first_radius - second_radius, linear_scale
        ) != 0) return {};
        if(first.radius == 0.0L) return {first.center};
        throw std::domain_error("coincident circles have infinitely many intersections");
    }

    if(geometry3d_scaled_sign(
        center_distance - first_radius - second_radius, linear_scale
    ) > 0 || geometry3d_scaled_sign(
        center_distance - std::abs(first_radius - second_radius), linear_scale
    ) < 0){
        return {};
    }
    const Point3 direction = difference / center_distance;
    const long double offset = (
        (first_radius - second_radius) * (first_radius + second_radius) +
        center_distance * center_distance
    ) / (2 * center_distance);
    const Point3 base = direction * offset;
    const long double squared_height =
        (first_radius - offset) * (first_radius + offset);
    const int height_sign = geometry3d_scaled_sign(
        squared_height,
        std::max(first_radius * first_radius, offset * offset)
    );
    if(height_sign < 0) return {};
    const auto restore = [&first, scale](const Point3& normalized){
        const Point3 result = first.center + normalized * scale;
        if(!geometry3d_is_finite(result))[[unlikely]]{
            throw std::overflow_error(
                "circle-circle intersection point is not representable"
            );
        }
        return result;
    };
    if(height_sign == 0) return {restore(base)};
    const Point3 perpendicular = cross(first_normal, direction);
    const Point3 delta = perpendicular * std::sqrt(
        std::max(0.0L, squared_height)
    );
    return {restore(base - delta), restore(base + delta)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_CIRCLE_INTERSECTIONS_HPP_INCLUDED
