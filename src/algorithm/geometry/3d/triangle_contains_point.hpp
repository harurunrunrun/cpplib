#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_CONTAINS_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_CONTAINS_POINT_HPP_INCLUDED

#include <algorithm>

#include "cross.hpp"
#include "geometry3d_sign.hpp"
#include "on_plane.hpp"
#include "plane3.hpp"
#include "point3.hpp"
#include "triangle3.hpp"
#include "triangle_point_barycentric_coordinates.hpp"

inline bool contains(const Triangle3& triangle, const Point3& point){
    const Plane3 plane{
        triangle.a,
        cross(triangle.b - triangle.a, triangle.c - triangle.a)
    };
    if(!on_plane(plane, point)) return false;
    const auto weights = barycentric_coordinates(triangle, point);
    return std::all_of(weights.begin(), weights.end(), [](long double weight){
        return geometry3d_sign(weight) >= 0;
    });
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_CONTAINS_POINT_HPP_INCLUDED
