#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_PLANE_INTERSECTIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_PLANE_INTERSECTIONS_HPP_INCLUDED

#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <vector>

#include "base.hpp"
#include "is_finite.hpp"
#include "geometry3d_sign.hpp"
#include "dot.hpp"
#include "line_circle_intersections.hpp"
#include "on_plane.hpp"
#include "parallel.hpp"
#include "plane3_unit_normal.hpp"
#include "plane_plane_intersection.hpp"

inline std::vector<Point3> circle_plane_intersections(
    const Circle3& circle,
    const Plane3& plane
){
    geometry3d_validate(circle);
    if(!geometry3d_is_finite(plane))[[unlikely]]{
        throw std::invalid_argument("circle-plane intersection requires a finite plane");
    }
    const Plane3 circle_plane{circle.center, circle.normal};
    const Point3 circle_normal = plane3_unit_normal(circle_plane);
    const Point3 plane_normal = plane3_unit_normal(plane);
    if(parallel(circle_normal, plane_normal)){
        const auto difference = geometry3d_normalized_difference(
            circle.center, plane.point, {circle.radius}
        );
        const Point3 offset = difference.value;
        const long double offset_length = std::hypot(
            offset.x, offset.y, offset.z
        );
        if(geometry3d_scaled_sign(
            dot(offset, plane_normal), offset_length
        ) != 0) return {};
        if(circle.radius == 0.0L) return {circle.center};
        throw std::domain_error("circle and plane have infinitely many intersections");
    }
    return line_circle_intersections(
        plane_plane_intersection(circle_plane, plane), circle
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_PLANE_INTERSECTIONS_HPP_INCLUDED
