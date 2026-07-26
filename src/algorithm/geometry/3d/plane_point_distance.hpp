#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_POINT_DISTANCE_HPP_INCLUDED

#include <cmath>

#include "plane3.hpp"
#include "point3.hpp"
#include "signed_distance.hpp"

inline long double distance(const Plane3& plane, const Point3& point){
    return std::abs(signed_distance(plane, point));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_POINT_DISTANCE_HPP_INCLUDED
