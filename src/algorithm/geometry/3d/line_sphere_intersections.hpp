#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SPHERE_INTERSECTIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SPHERE_INTERSECTIONS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "is_finite.hpp"
#include "geometry3d_sign.hpp"
#include "dot.hpp"

namespace geometry3d_detail{

struct LineSphereIntersectionParameters{
    Point3 unit_direction;
    Point3 normalized_center_offset;
    long double scale;
    std::vector<long double> signed_distances;
};

inline LineSphereIntersectionParameters line_sphere_intersection_parameters(
    const Line3& line,
    const Sphere3& sphere
){
    geometry3d_validate(sphere);
    if(!geometry3d_is_finite(line))[[unlikely]]{
        throw std::invalid_argument("line-sphere intersection requires a finite line");
    }
    const Geometry3DNormalizedDifference direction =
        geometry3d_normalized_difference(line.b, line.a);
    const long double direction_length = std::hypot(
        direction.value.x, direction.value.y, direction.value.z
    );
    if(direction_length == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D line");
    }
    const Point3 unit_direction = direction.value / direction_length;

    const Geometry3DNormalizedDifference center_offset =
        geometry3d_normalized_difference(
            sphere.center, line.a, {sphere.radius}
        );
    const long double radius = sphere.radius / center_offset.scale;
    const long double along = dot(
        center_offset.value, unit_direction
    );
    const Point3 perpendicular =
        unit_direction * along - center_offset.value;
    const long double perpendicular_distance = std::hypot(
        perpendicular.x, perpendicular.y, perpendicular.z
    );
    const long double squared_height =
        (radius - perpendicular_distance) *
        (radius + perpendicular_distance);
    const int sign = geometry3d_scaled_sign(
        squared_height,
        std::max(
            radius * radius,
            perpendicular_distance * perpendicular_distance
        )
    );
    LineSphereIntersectionParameters result{
        unit_direction,
        center_offset.value,
        center_offset.scale,
        {},
    };
    if(sign < 0) return result;
    if(sign == 0){
        result.signed_distances.push_back(along);
        return result;
    }
    const long double height = std::sqrt(
        std::max(0.0L, squared_height)
    );
    result.signed_distances = {along - height, along + height};
    return result;
}

inline Point3 restore_line_sphere_intersection(
    const Point3& origin,
    const LineSphereIntersectionParameters& parameters,
    long double normalized_signed_distance
){
    const Point3 result{
        std::fma(
            parameters.unit_direction.x * normalized_signed_distance,
            parameters.scale,
            origin.x
        ),
        std::fma(
            parameters.unit_direction.y * normalized_signed_distance,
            parameters.scale,
            origin.y
        ),
        std::fma(
            parameters.unit_direction.z * normalized_signed_distance,
            parameters.scale,
            origin.z
        ),
    };
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error(
            "line-sphere intersection point is not representable"
        );
    }
    return result;
}

}

inline std::vector<Point3> line_sphere_intersections(
    const Line3& line,
    const Sphere3& sphere
){
    const geometry3d_detail::LineSphereIntersectionParameters parameters =
        geometry3d_detail::line_sphere_intersection_parameters(line, sphere);
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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SPHERE_INTERSECTIONS_HPP_INCLUDED
