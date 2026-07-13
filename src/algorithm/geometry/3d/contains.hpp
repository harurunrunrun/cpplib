#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "barycentric_coordinates.hpp"
#include "cross.hpp"
#include "geometry3d_sign.hpp"
#include "on_plane.hpp"

inline bool contains(const Triangle3& triangle, const Point3& point){
    const Plane3 plane{triangle.a, cross(
        triangle.b - triangle.a, triangle.c - triangle.a
    )};
    if(!on_plane(plane, point)) return false;
    const auto weights = barycentric_coordinates(triangle, point);
    return std::all_of(weights.begin(), weights.end(), [](long double weight){
        return geometry3d_sign(weight) >= 0;
    });
}

inline bool contains(const Tetrahedron3& tetrahedron, const Point3& point){
    const auto weights = barycentric_coordinates(tetrahedron, point);
    return std::all_of(weights.begin(), weights.end(), [](long double weight){
        return geometry3d_sign(weight) >= 0;
    });
}

inline int contains(const Sphere3& sphere, const Point3& point){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    return -geometry3d_sign(abs(point - sphere.center) - sphere.radius);
}
