#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_STRONG_COMPONENT_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_STRONG_COMPONENT_COUNT_HPP_INCLUDED

#include <vector>

#include "iterative_strongly_connected_components.hpp"

inline int strong_component_count(
    const std::vector<std::vector<int>>& graph
){
    return iterative_strongly_connected_components(graph).count;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_STRONG_COMPONENT_COUNT_HPP_INCLUDED
