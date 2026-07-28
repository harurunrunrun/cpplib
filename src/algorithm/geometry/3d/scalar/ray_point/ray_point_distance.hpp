#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_RAY_POINT_RAY_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_RAY_POINT_RAY_POINT_DISTANCE_HPP_INCLUDED

#include "../../aggregate/all/closest_point.hpp"
#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../type/definition/point3.hpp"
#include "../../type/definition/ray3.hpp"

inline long double distance(const Ray3& ray, const Point3& point){
    return geometry3d_linear_closest_detail::checked_distance(
        point, closest_point(ray, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_RAY_POINT_RAY_POINT_DISTANCE_HPP_INCLUDED
