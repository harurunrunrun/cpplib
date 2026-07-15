#pragma once

#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "circle_circle_intersections.hpp"
#include "plane3_unit_normal.hpp"
#include "plane_sphere_intersection.hpp"

inline std::vector<Point3> circle_sphere_intersections(
    const Circle3& circle,
    const Sphere3& sphere
){
    if(circle.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    (void)plane3_unit_normal({circle.center, circle.normal});
    const auto section = plane_sphere_intersection(
        Plane3{circle.center, circle.normal}, sphere
    );
    if(!section) return {};
    return circle_circle_intersections(circle, *section);
}
