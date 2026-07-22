#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_PLANE_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_PLANE_DISTANCE_HPP_INCLUDED

#include "abs.hpp"
#include "base.hpp"
#include "plane_plane_closest_points.hpp"

inline long double distance(const Plane3& first, const Plane3& second){
    const auto points = closest_points(first, second);
    return abs(points.first - points.second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_PLANE_DISTANCE_HPP_INCLUDED
