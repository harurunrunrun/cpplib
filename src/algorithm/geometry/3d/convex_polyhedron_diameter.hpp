#pragma once

#include <optional>

#include "convex_polyhedron3.hpp"
#include "point_set_diameter_3d.hpp"

inline std::optional<PointSetDiameter3DResult> convex_polyhedron_diameter(
    const ConvexPolyhedron3& polyhedron
){
    return point_set_diameter_3d(polyhedron.vertices);
}
