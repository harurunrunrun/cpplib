#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_FACET3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_FACET3_HPP_INCLUDED

#include <cstddef>
#include <vector>

struct ConvexPolyhedronFacet3{
    std::vector<std::size_t> boundary;
    std::vector<std::size_t> triangles;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_FACET3_HPP_INCLUDED
