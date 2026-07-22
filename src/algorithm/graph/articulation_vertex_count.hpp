#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_ARTICULATION_VERTEX_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_ARTICULATION_VERTEX_COUNT_HPP_INCLUDED

#include <utility>
#include <vector>

#include "lowlink.hpp"

inline int articulation_vertex_count(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    return static_cast<int>(lowlink(vertex_count, edges).articulation.size());
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_ARTICULATION_VERTEX_COUNT_HPP_INCLUDED
