#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_COUNTS3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_COUNTS3_HPP_INCLUDED

#include <cstddef>

struct ConvexPolyhedronCounts3{
    std::size_t vertices = 0;
    std::size_t edges = 0;
    // Backward-compatible name for triangulated_face_count.
    std::size_t triangular_faces = 0;
    std::size_t facet_count = 0;
    std::size_t triangulated_face_count = 0;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_COUNTS3_HPP_INCLUDED
