#pragma once

#include <cstddef>

#include "convex_polyhedron_edges.hpp"
#include "convex_polyhedron_facets.hpp"

struct ConvexPolyhedronCounts3{
    std::size_t vertices = 0;
    std::size_t edges = 0;
    // Backward-compatible name for triangulated_face_count.
    std::size_t triangular_faces = 0;
    std::size_t facet_count = 0;
    std::size_t triangulated_face_count = 0;
};

inline ConvexPolyhedronCounts3 convex_polyhedron_counts(
    const ConvexPolyhedron3& polyhedron
){
    const std::size_t triangulated = polyhedron.faces.size();
    return {
        polyhedron.vertices.size(), convex_polyhedron_edges(polyhedron).size(),
        triangulated, convex_polyhedron_facets(polyhedron).size(), triangulated,
    };
}
