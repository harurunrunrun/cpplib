#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SPHERE_INTERSECTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SPHERE_INTERSECTS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "convex_polyhedron_point_distance.hpp"
#include "is_finite.hpp"

inline bool convex_polyhedron_sphere_intersects(
    const ConvexPolyhedron3& polyhedron,
    const Sphere3& sphere
){
    geometry3d_validate(sphere);
    if(polyhedron.vertices.empty())[[unlikely]]{
        throw std::invalid_argument(
            "sphere intersection with empty convex polyhedron"
        );
    }

    std::vector<Geometry3DNormalizedDifference> offsets;
    offsets.reserve(polyhedron.vertices.size());
    long double scale = 0.0L;
    for(const Point3& vertex: polyhedron.vertices){
        offsets.push_back(
            geometry3d_normalized_difference(
                vertex, sphere.center, {sphere.radius}
            )
        );
        scale = std::max(scale, offsets.back().scale);
    }

    ConvexPolyhedron3 normalized = polyhedron;
    normalized.vertices.clear();
    normalized.vertices.reserve(offsets.size());
    for(const Geometry3DNormalizedDifference& offset: offsets){
        normalized.vertices.push_back(
            offset.value * (offset.scale / scale)
        );
    }
    return convex_polyhedron_point_distance(normalized, {0.0L, 0.0L, 0.0L})
        <= sphere.radius / scale;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SPHERE_INTERSECTS_HPP_INCLUDED
