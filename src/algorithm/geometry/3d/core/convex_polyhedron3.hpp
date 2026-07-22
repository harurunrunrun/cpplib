#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CORE_CONVEX_POLYHEDRON3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CORE_CONVEX_POLYHEDRON3_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <vector>

#include "geometry_primitives.hpp"

struct ConvexPolyhedron3{
    int affine_dimension = -1;
    std::vector<Point3> vertices;
    std::vector<std::array<std::size_t, 3>> faces;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CORE_CONVEX_POLYHEDRON3_HPP_INCLUDED
