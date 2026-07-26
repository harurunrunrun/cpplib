#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_VORONOI_EDGE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_VORONOI_EDGE_HPP_INCLUDED

#include <cstddef>
#include <limits>

#include "point.hpp"
#include "voronoi_edge_kind.hpp"

inline constexpr std::size_t VORONOI_NO_VERTEX =
    std::numeric_limits<std::size_t>::max();

struct VoronoiEdge{
    VoronoiEdgeKind kind = VoronoiEdgeKind::SEGMENT;
    std::size_t first_site = 0;
    std::size_t second_site = 0;
    Point origin;
    Point endpoint_or_direction;
    std::size_t first_vertex = VORONOI_NO_VERTEX;
    std::size_t second_vertex = VORONOI_NO_VERTEX;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_VORONOI_EDGE_HPP_INCLUDED
