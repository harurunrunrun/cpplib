#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_CONTAINS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_CONTAINS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "is_finite.hpp"
#include "../scalar/abs.hpp"
#include "../query/barycentric_coordinates.hpp"
#include "../point/cross.hpp"
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
    geometry3d_validate(sphere);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("sphere containment requires a finite point");
    }
    const auto difference = geometry3d_normalized_difference(
        point, sphere.center, {sphere.radius}
    );
    const long double center_distance = std::hypot(
        difference.value.x, difference.value.y, difference.value.z
    );
    const long double radius = sphere.radius / difference.scale;
    return -geometry3d_scaled_sign(
        center_distance - radius, std::max(center_distance, radius)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_CONTAINS_HPP_INCLUDED
