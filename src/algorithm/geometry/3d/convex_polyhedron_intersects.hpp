#pragma once

#include "convex_polyhedron_intersection.hpp"

inline bool convex_polyhedron_intersects(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    return !convex_polyhedron_intersection(first, second).vertices.empty();
}
