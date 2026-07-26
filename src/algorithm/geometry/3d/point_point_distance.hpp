#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POINT_DISTANCE_HPP_INCLUDED

#include "linear_closest_detail.hpp"
#include "point3.hpp"

inline long double distance(const Point3& left, const Point3& right){
    return geometry3d_linear_closest_detail::checked_distance(left, right);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POINT_DISTANCE_HPP_INCLUDED
