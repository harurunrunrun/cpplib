#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_RAY_TETRAHEDRON_RAY_TETRAHEDRON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_RAY_TETRAHEDRON_RAY_TETRAHEDRON_INTERSECTION_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../line_tetrahedron/line_tetrahedron_intersection.hpp"
#include "../../type/definition/linear_intersection3.hpp"
#include "../../predicate/ray_point/on_ray.hpp"
#include "../../point/ray/ray3_direction.hpp"
#include "../ray_segment/ray_segment_intersection.hpp"

inline LinearIntersection3 ray_tetrahedron_intersection(
    const Ray3& ray,
    const Tetrahedron3& tetrahedron
){
    (void)ray3_direction(ray);
    const LinearIntersection3 support = line_tetrahedron_intersection(
        Line3{ray.origin, ray.through}, tetrahedron
    );
    if(const auto* point = std::get_if<Point3>(&support)){
        return on_ray(ray, *point) ? LinearIntersection3{*point}
            : LinearIntersection3{std::monostate{}};
    }
    if(const auto* segment = std::get_if<Segment3>(&support)){
        return ray_segment_intersection(ray, *segment);
    }
    return std::monostate{};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_RAY_TETRAHEDRON_RAY_TETRAHEDRON_INTERSECTION_HPP_INCLUDED
