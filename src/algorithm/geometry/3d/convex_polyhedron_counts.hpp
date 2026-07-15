#pragma once

#include <cstddef>

#include "convex_polyhedron_edges.hpp"

struct ConvexPolyhedronCounts3{
    std::size_t vertices = 0;
    std::size_t edges = 0;
    std::size_t triangular_faces = 0;
};

inline ConvexPolyhedronCounts3 convex_polyhedron_counts(
    const ConvexPolyhedron3& polyhedron
){
    return {
        polyhedron.vertices.size(), convex_polyhedron_edges(polyhedron).size(),
        polyhedron.faces.size(),
    };
}
