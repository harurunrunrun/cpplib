#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POINT_POLYHEDRON_CONTAINS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POINT_POLYHEDRON_CONTAINS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../../constant/definition/constants.hpp"
#include "../../point/point_point/point3_safe_normalized_difference.hpp"
#include "../../result/point_point/point3_normalized_difference_points.hpp"
#include "../../scalar/point_point/dot.hpp"
#include "../../scalar/point_point_point/scalar_triple.hpp"
#include "../../scalar/triangle_point/triangle_point_distance.hpp"
#include "../../type/definition/polyhedron3.hpp"
#include "../../void/polyhedron/polyhedron3_validate.hpp"

namespace polyhedron_contains_detail{

inline bool unchecked(const Polyhedron3& polyhedron, const Point3& point){
    if(polyhedron.faces.empty()) return false;
    long double solid_angle = 0.0L;
    for(const auto& face: polyhedron.faces){
        const Triangle3 triangle{
            polyhedron.vertices[face[0]], polyhedron.vertices[face[1]],
            polyhedron.vertices[face[2]]
        };
        if(distance(triangle, point) <= GEOMETRY3D_EPS) return true;

        const Geometry3DNormalizedDifference first_difference =
            geometry3d_normalized_difference(triangle.a, point);
        const Geometry3DNormalizedDifference second_difference =
            geometry3d_normalized_difference(triangle.b, point);
        const Geometry3DNormalizedDifference third_difference =
            geometry3d_normalized_difference(triangle.c, point);
        const long double scale = std::max({
            first_difference.scale, second_difference.scale,
            third_difference.scale,
        });
        const Point3 first = geometry3d_safe_normalized_difference(
            triangle.a, point, scale
        );
        const Point3 second = geometry3d_safe_normalized_difference(
            triangle.b, point, scale
        );
        const Point3 third = geometry3d_safe_normalized_difference(
            triangle.c, point, scale
        );
        const long double first_norm = std::hypot(first.x, first.y, first.z);
        const long double second_norm = std::hypot(second.x, second.y, second.z);
        const long double third_norm = std::hypot(third.x, third.y, third.z);
        const long double denominator =
            first_norm * second_norm * third_norm
            + dot(first, second) * third_norm
            + dot(second, third) * first_norm
            + dot(third, first) * second_norm;
        solid_angle += 2.0L * std::atan2(
            scalar_triple(first, second, third), denominator
        );
    }
    return std::abs(solid_angle) > 2.0L * GEOMETRY3D_PI;
}

}  // namespace polyhedron_contains_detail

inline bool polyhedron_contains(
    const Polyhedron3& polyhedron,
    const Point3& point
){
    geometry3d_validate(polyhedron);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("non-finite polyhedron query point");
    }
    return polyhedron_contains_detail::unchecked(polyhedron, point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POINT_POLYHEDRON_CONTAINS_HPP_INCLUDED
