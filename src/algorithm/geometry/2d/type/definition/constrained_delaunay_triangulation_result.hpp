#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_CONSTRAINED_DELAUNAY_TRIANGULATION_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_CONSTRAINED_DELAUNAY_TRIANGULATION_RESULT_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

struct ConstrainedDelaunayTriangulationResult{
    std::vector<std::array<std::size_t, 3>> triangles;
    std::vector<std::pair<std::size_t, std::size_t>> edges;
    std::vector<std::pair<std::size_t, std::size_t>> constraints;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_CONSTRAINED_DELAUNAY_TRIANGULATION_RESULT_HPP_INCLUDED
