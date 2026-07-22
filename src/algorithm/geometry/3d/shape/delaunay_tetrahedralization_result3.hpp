#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_DELAUNAY_TETRAHEDRALIZATION_RESULT3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_DELAUNAY_TETRAHEDRALIZATION_RESULT3_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <vector>

#include "../core/geometry_primitives.hpp"

struct DelaunayTetrahedralization3{
    int affine_dimension = -1;
    std::vector<Point3> vertices;
    std::vector<std::array<std::size_t, 4>> tetrahedra;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_DELAUNAY_TETRAHEDRALIZATION_RESULT3_HPP_INCLUDED
