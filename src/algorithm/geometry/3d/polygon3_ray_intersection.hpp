#pragma once

#include <limits>

#include "polygon3_line_intersection.hpp"
#include "ray3_direction.hpp"

inline Polygon3LinearIntersection3 polygon3_ray_intersection(
    const Polygon3& polygon,
    const Ray3& ray
){
    (void)ray3_direction(ray);
    return polygon3_line_intersection_detail::intersect(
        polygon, {ray.origin, ray.through}, 0.0L,
        std::numeric_limits<long double>::infinity()
    );
}
