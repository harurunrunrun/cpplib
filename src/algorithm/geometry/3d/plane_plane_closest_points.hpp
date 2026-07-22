#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_PLANE_CLOSEST_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_PLANE_CLOSEST_POINTS_HPP_INCLUDED

#include <utility>

#include "base.hpp"
#include "parallel.hpp"
#include "plane3_unit_normal.hpp"
#include "plane_plane_intersection.hpp"
#include "projection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Plane3& first,
    const Plane3& second
){
    const Point3 first_normal = plane3_unit_normal(first);
    const Point3 second_normal = plane3_unit_normal(second);
    if(!parallel(first_normal, second_normal)){
        const Point3 point = plane_plane_intersection(first, second).a;
        return {point, point};
    }
    return {first.point, projection(second, first.point)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_PLANE_CLOSEST_POINTS_HPP_INCLUDED
