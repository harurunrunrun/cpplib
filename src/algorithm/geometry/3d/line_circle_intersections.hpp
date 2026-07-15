#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_finite.hpp"
#include "line_sphere_intersections.hpp"
#include "plane3_unit_normal.hpp"

namespace geometry3d_detail{

inline LineSphereIntersectionParameters line_circle_intersection_parameters(
    const Line3& line,
    const Circle3& circle
){
    geometry3d_validate(circle);
    const Point3 unit_normal = plane3_unit_normal(
        {circle.center, circle.normal}
    );
    LineSphereIntersectionParameters parameters =
        line_sphere_intersection_parameters(
            line, {circle.center, circle.radius}
        );
    const long double normalized_radius =
        circle.radius / parameters.scale;
    std::vector<long double> filtered;
    filtered.reserve(parameters.signed_distances.size());
    for(const long double signed_distance: parameters.signed_distances){
        const Point3 normalized_offset =
            parameters.unit_direction * signed_distance -
            parameters.normalized_center_offset;
        const long double characteristic = std::max(
            normalized_radius,
            std::hypot(
                normalized_offset.x,
                normalized_offset.y,
                normalized_offset.z
            )
        );
        if(geometry3d_scaled_sign(
            dot(normalized_offset, unit_normal), characteristic
        ) == 0){
            filtered.push_back(signed_distance);
        }
    }
    parameters.signed_distances = std::move(filtered);
    return parameters;
}

}

inline std::vector<Point3> line_circle_intersections(
    const Line3& line,
    const Circle3& circle
){
    const geometry3d_detail::LineSphereIntersectionParameters parameters =
        geometry3d_detail::line_circle_intersection_parameters(line, circle);
    std::vector<Point3> result;
    result.reserve(parameters.signed_distances.size());
    for(const long double signed_distance: parameters.signed_distances){
        result.push_back(
            geometry3d_detail::restore_line_sphere_intersection(
                line.a, parameters, signed_distance
            )
        );
    }
    return result;
}
