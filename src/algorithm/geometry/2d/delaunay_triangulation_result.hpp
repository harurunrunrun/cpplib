#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DELAUNAY_TRIANGULATION_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DELAUNAY_TRIANGULATION_RESULT_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

struct DelaunayTriangulationResult {
    std::vector<std::array<std::size_t, 3>> triangles;
    std::vector<std::pair<std::size_t, std::size_t>> edges;
    std::vector<std::size_t> representative;
    std::vector<std::size_t> sites;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DELAUNAY_TRIANGULATION_RESULT_HPP_INCLUDED
