#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_CIRCLE_INTERSECTIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_CIRCLE_INTERSECTIONS_HPP_INCLUDED

#include <cmath>
#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "is_finite.hpp"
#include "line_circle_intersections.hpp"
#include "on_circle.hpp"

inline std::vector<Point3> segment_circle_intersections(
    const Segment3& segment,
    const Circle3& circle
){
    geometry3d_validate(circle);
    if(!geometry3d_is_finite(segment))[[unlikely]]{
        throw std::invalid_argument(
            "segment-circle intersection requires a finite segment"
        );
    }
    if(segment.a.x == segment.b.x && segment.a.y == segment.b.y &&
        segment.a.z == segment.b.z){
        return on_circle(circle, segment.a)
            ? std::vector<Point3>{segment.a} : std::vector<Point3>{};
    }

    const Geometry3DNormalizedDifference segment_direction =
        geometry3d_normalized_difference(segment.b, segment.a);
    const long double normalized_segment_length = std::hypot(
        segment_direction.value.x,
        segment_direction.value.y,
        segment_direction.value.z
    );
    const geometry3d_detail::LineSphereIntersectionParameters parameters =
        geometry3d_detail::line_circle_intersection_parameters(
            {segment.a, segment.b}, circle
        );

    std::vector<Point3> result;
    result.reserve(parameters.signed_distances.size());
    for(const long double signed_distance: parameters.signed_distances){
        const int lower_sign = geometry3d_sign(signed_distance);
        if(lower_sign < 0) continue;
        if(signed_distance < 0.0L){
            result.push_back(segment.a);
            continue;
        }
        const int upper_comparison = geometry3d_compare_nonnegative_scaled(
            signed_distance,
            parameters.scale,
            normalized_segment_length * (1.0L + GEOMETRY3D_EPS),
            segment_direction.scale
        );
        if(upper_comparison > 0) continue;
        const int exact_upper_comparison = geometry3d_compare_nonnegative_scaled(
            signed_distance,
            parameters.scale,
            normalized_segment_length,
            segment_direction.scale
        );
        if(exact_upper_comparison > 0){
            result.push_back(segment.b);
        }else{
            result.push_back(
                geometry3d_detail::restore_line_sphere_intersection(
                    segment.a, parameters, signed_distance
                )
            );
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_CIRCLE_INTERSECTIONS_HPP_INCLUDED
