#pragma once

#include <utility>
#include <vector>

#include "cycle_detection.hpp"

inline CycleDetectionResult undirected_cycle_certificate(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    return undirected_cycle_with_edges(vertex_count, edges);
}
