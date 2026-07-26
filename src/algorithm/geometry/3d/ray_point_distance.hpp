#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_POINT_DISTANCE_HPP_INCLUDED

#include "closest_point.hpp"
#include "linear_closest_detail.hpp"
#include "point3.hpp"
#include "ray3.hpp"

inline long double distance(const Ray3& ray, const Point3& point){
    return geometry3d_linear_closest_detail::checked_distance(
        point, closest_point(ray, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_POINT_DISTANCE_HPP_INCLUDED
