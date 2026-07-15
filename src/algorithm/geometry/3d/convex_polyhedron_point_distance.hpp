#pragma once

#include "convex_polyhedron_closest_point.hpp"
#include "distance.hpp"

inline long double convex_polyhedron_point_distance(
    const ConvexPolyhedron3& polyhedron,
    const Point3& point
){
    return distance(convex_polyhedron_closest_point(polyhedron, point), point);
}
