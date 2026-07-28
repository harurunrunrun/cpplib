#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_PLANE_POINT_PLANE_POINT_PROJECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_PLANE_POINT_PLANE_POINT_PROJECTION_HPP_INCLUDED

#include <cmath>

#include "../../constant/definition/constants.hpp"
#include "../../scalar/point_point/dot.hpp"
#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../predicate/plane_point/on_plane.hpp"
#include "../../type/definition/plane3.hpp"
#include "../plane/plane3_unit_normal.hpp"
#include "../../type/definition/point3.hpp"

inline Point3 projection(const Plane3& plane, const Point3& point){
    const Point3 normal = plane3_unit_normal(plane);
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(point, plane.point);
    const long double parameter = dot(difference.value, normal);
    if(parameter == 0.0L
        || (std::abs(parameter) <= GEOMETRY3D_EPS
                * std::hypot(
                    difference.value.x,
                    difference.value.y,
                    difference.value.z
                )
            && geometry3d_adaptive_detail::sign(
                geometry3d_plane_numeric_detail::exact_dot_difference(
                    plane.normal, point, plane.point
                ).value
            ) == 0)){
        return point;
    }
    const Point3 local = difference.value - normal * parameter;
    return geometry3d_linear_closest_detail::restore(
        local, plane.point, difference.scale
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_PLANE_POINT_PLANE_POINT_PROJECTION_HPP_INCLUDED
