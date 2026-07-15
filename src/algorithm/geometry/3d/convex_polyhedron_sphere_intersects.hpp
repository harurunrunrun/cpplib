#pragma once

#include <cmath>
#include <stdexcept>

#include "convex_polyhedron_point_distance.hpp"

inline bool convex_polyhedron_sphere_intersects(
    const ConvexPolyhedron3& polyhedron,
    const Sphere3& sphere
){
    if(!std::isfinite(sphere.radius) || sphere.radius < 0.0L)[[unlikely]]{
        throw std::invalid_argument("invalid sphere in convex polyhedron intersection");
    }
    return convex_polyhedron_point_distance(polyhedron, sphere.center)
        <= sphere.radius;
}
