#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYHEDRON3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYHEDRON3_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <vector>

#include "point3.hpp"

struct Polyhedron3{
    std::vector<Point3> vertices;
    std::vector<std::array<std::size_t, 3>> faces;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYHEDRON3_HPP_INCLUDED
