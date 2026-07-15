#pragma once

#include "convex_polyhedron_support_point.hpp"

inline Plane3 convex_polyhedron_support_plane(
    const ConvexPolyhedron3& polyhedron,
    const Point3& outward_normal
){
    return {
        convex_polyhedron_support_point(polyhedron, outward_normal),
        outward_normal,
    };
}
