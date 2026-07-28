#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_POINT_POINT_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_POINT_POINT_POINT_DISTANCE_HPP_INCLUDED

#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../type/definition/point3.hpp"

inline long double distance(const Point3& left, const Point3& right){
    return geometry3d_linear_closest_detail::checked_distance(left, right);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_POINT_POINT_POINT_DISTANCE_HPP_INCLUDED
