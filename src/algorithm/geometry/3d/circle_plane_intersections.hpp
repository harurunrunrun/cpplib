#pragma once

#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "line_circle_intersections.hpp"
#include "on_plane.hpp"
#include "parallel.hpp"
#include "plane3_unit_normal.hpp"
#include "plane_plane_intersection.hpp"

inline std::vector<Point3> circle_plane_intersections(
    const Circle3& circle,
    const Plane3& plane
){
    if(circle.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    const Plane3 circle_plane{circle.center, circle.normal};
    const Point3 circle_normal = plane3_unit_normal(circle_plane);
    const Point3 plane_normal = plane3_unit_normal(plane);
    if(parallel(circle_normal, plane_normal)){
        if(!on_plane(plane, circle.center)) return {};
        if(geometry3d_sign(circle.radius) == 0) return {circle.center};
        throw std::domain_error("circle and plane have infinitely many intersections");
    }
    return line_circle_intersections(
        plane_plane_intersection(circle_plane, plane), circle
    );
}
