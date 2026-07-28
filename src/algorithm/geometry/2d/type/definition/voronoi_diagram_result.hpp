#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_VORONOI_DIAGRAM_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_VORONOI_DIAGRAM_RESULT_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "point.hpp"
#include "voronoi_edge.hpp"

struct VoronoiDiagramResult{
    std::vector<Point> vertices;
    std::vector<VoronoiEdge> edges;
    std::vector<std::vector<std::size_t>> cell_edges;
    std::vector<std::size_t> representative;
    std::vector<std::size_t> sites;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_VORONOI_DIAGRAM_RESULT_HPP_INCLUDED
