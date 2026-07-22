#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_PLANE_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_PLANE_PLANE_COMMON_INTERSECTION_HPP_INCLUDED

#include <stdexcept>
#include <variant>

#include "../predicate/is_finite.hpp"
#include "../predicate/on_plane.hpp"
#include "../predicate/parallel.hpp"
#include "../point/plane3_unit_normal.hpp"
#include "../shape/plane_plane_intersection.hpp"

using PlanePlaneIntersection3 = std::variant<std::monostate, Line3, Plane3>;

inline PlanePlaneIntersection3 plane_plane_common_intersection(
    const Plane3& first,
    const Plane3& second
){
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second))
        [[unlikely]]{
        throw std::invalid_argument(
            "plane_plane_common_intersection requires finite planes"
        );
    }
    const Point3 first_normal = plane3_unit_normal(first);
    const Point3 second_normal = plane3_unit_normal(second);
    if(parallel(first_normal, second_normal)){
        if(on_plane(first, second.point)) return first;
        return std::monostate{};
    }
    return plane_plane_intersection(first, second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_PLANE_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
