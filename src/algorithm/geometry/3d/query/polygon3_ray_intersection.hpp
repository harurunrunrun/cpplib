#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_POLYGON3_RAY_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_POLYGON3_RAY_INTERSECTION_HPP_INCLUDED

#include <limits>

#include "polygon3_line_intersection.hpp"
#include "../point/ray3_direction.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_POLYGON3_RAY_INTERSECTION_HPP_INCLUDED
