#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_STRAIGHT_SKELETON_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_STRAIGHT_SKELETON_RESULT_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "point.hpp"

enum class StraightSkeletonVertexKind : std::uint8_t{
    contour_vertex,
    edge_event,
    split_event,
    peak_event,
    simultaneous_event,
};

struct StraightSkeletonResult{
    std::vector<Point> vertices;
    std::vector<long double> times;
    std::vector<StraightSkeletonVertexKind> kinds;
    std::vector<std::pair<std::size_t, std::size_t>> edges;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_STRAIGHT_SKELETON_RESULT_HPP_INCLUDED
