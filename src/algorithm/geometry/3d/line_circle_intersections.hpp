#pragma once

#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "line_sphere_intersections.hpp"
#include "on_plane.hpp"
#include "plane3_unit_normal.hpp"

inline std::vector<Point3> line_circle_intersections(
    const Line3& line,
    const Circle3& circle
){
    if(circle.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    const Plane3 circle_plane{circle.center, circle.normal};
    (void)plane3_unit_normal(circle_plane);
    std::vector<Point3> result;
    for(const Point3& point: line_sphere_intersections(
        line, Sphere3{circle.center, circle.radius}
    )){
        if(on_plane(circle_plane, point)) result.push_back(point);
    }
    return result;
}
