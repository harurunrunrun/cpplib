#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_PLANE_PLANE_PLANE_PLANE_COMMON_INTERSECTION_PLANES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_PLANE_PLANE_PLANE_PLANE_COMMON_INTERSECTION_PLANES_HPP_INCLUDED

#include <stdexcept>

#include "../plane/plane_plane_common_intersection_result3.hpp"
#include "../../aggregate/all/is_finite.hpp"
#include "../../predicate/plane_point/on_plane.hpp"
#include "../../predicate/point_point/parallel.hpp"
#include "../../point/plane/plane3_unit_normal.hpp"
#include "../../line/plane_plane/plane_plane_intersection.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_PLANE_PLANE_PLANE_PLANE_COMMON_INTERSECTION_PLANES_HPP_INCLUDED
