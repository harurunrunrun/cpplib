#pragma once

#include <vector>

#include "iterative_strongly_connected_components.hpp"

inline int strong_component_count(
    const std::vector<std::vector<int>>& graph
){
    return iterative_strongly_connected_components(graph).count;
}
