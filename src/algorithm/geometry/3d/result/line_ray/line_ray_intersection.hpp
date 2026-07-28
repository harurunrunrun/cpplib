#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_LINE_RAY_LINE_RAY_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_LINE_RAY_LINE_RAY_INTERSECTION_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../line_line/line_line_intersection.hpp"
#include "../../type/definition/linear_intersection3.hpp"
#include "../../predicate/ray_point/on_ray.hpp"
#include "../../point/ray/ray3_direction.hpp"

inline LinearIntersection3 line_ray_intersection(
    const Line3& line,
    const Ray3& ray
){
    (void)ray3_direction(ray);
    const LinearIntersection3 support = line_line_intersection(
        line, Line3{ray.origin, ray.through}
    );
    if(const auto* point = std::get_if<Point3>(&support)){
        return on_ray(ray, *point) ? LinearIntersection3{*point}
            : LinearIntersection3{std::monostate{}};
    }
    if(std::holds_alternative<Line3>(support)) return ray;
    return std::monostate{};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_LINE_RAY_LINE_RAY_INTERSECTION_HPP_INCLUDED
