#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_COUNTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_COUNTS_HPP_INCLUDED

#include <cstddef>

#include "convex_polyhedron_counts3.hpp"
#include "convex_polyhedron_edges.hpp"
#include "convex_polyhedron_facets.hpp"

inline ConvexPolyhedronCounts3 convex_polyhedron_counts(
    const ConvexPolyhedron3& polyhedron
){
    const std::size_t triangulated = polyhedron.faces.size();
    return {
        polyhedron.vertices.size(), convex_polyhedron_edges(polyhedron).size(),
        triangulated, convex_polyhedron_facets(polyhedron).size(), triangulated,
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_COUNTS_HPP_INCLUDED
