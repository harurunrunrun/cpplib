#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "abs.hpp"
#include "base.hpp"
#include "cross.hpp"
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
    if(first.radius < 0 || second.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
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
    if(!on_plane(first_plane, second.center)) return {};

    const Point3 difference = second.center - first.center;
    const long double center_distance = abs(difference);
    if(geometry3d_sign(center_distance) == 0){
        if(geometry3d_sign(first.radius - second.radius) != 0) return {};
        if(geometry3d_sign(first.radius) == 0) return {first.center};
        throw std::domain_error("coincident circles have infinitely many intersections");
    }

    if(geometry3d_sign(center_distance - first.radius - second.radius) > 0 ||
        geometry3d_sign(
            center_distance - std::abs(first.radius - second.radius)
        ) < 0){
        return {};
    }
    const Point3 direction = difference / center_distance;
    const long double offset = (
        first.radius * first.radius - second.radius * second.radius +
        center_distance * center_distance
    ) / (2 * center_distance);
    const Point3 base = first.center + direction * offset;
    const long double squared_height =
        first.radius * first.radius - offset * offset;
    const int height_sign = geometry3d_sign(squared_height);
    if(height_sign < 0) return {};
    if(height_sign == 0) return {base};
    const Point3 perpendicular = cross(first_normal, direction);
    const Point3 delta = perpendicular * std::sqrt(
        std::max(0.0L, squared_height)
    );
    return {base - delta, base + delta};
}
