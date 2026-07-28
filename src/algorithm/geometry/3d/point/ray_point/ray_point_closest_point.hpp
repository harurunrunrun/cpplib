#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_RAY_POINT_RAY_POINT_CLOSEST_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_RAY_POINT_RAY_POINT_CLOSEST_POINT_HPP_INCLUDED

#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../type/definition/point3.hpp"
#include "../../type/definition/ray3.hpp"

inline Point3 closest_point(const Ray3& ray, const Point3& point){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {ray.origin, ray.through, Kind::ray},
        {point, point, Kind::segment}
    ).first;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_RAY_POINT_RAY_POINT_CLOSEST_POINT_HPP_INCLUDED
