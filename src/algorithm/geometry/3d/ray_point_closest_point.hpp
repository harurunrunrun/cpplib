#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_POINT_CLOSEST_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_POINT_CLOSEST_POINT_HPP_INCLUDED

#include "linear_closest_detail.hpp"
#include "point3.hpp"
#include "ray3.hpp"

inline Point3 closest_point(const Ray3& ray, const Point3& point){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {ray.origin, ray.through, Kind::ray},
        {point, point, Kind::segment}
    ).first;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_POINT_CLOSEST_POINT_HPP_INCLUDED
