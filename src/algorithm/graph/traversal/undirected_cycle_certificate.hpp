#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_UNDIRECTED_CYCLE_CERTIFICATE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_UNDIRECTED_CYCLE_CERTIFICATE_HPP_INCLUDED

#include <utility>
#include <vector>

#include "cycle_detection.hpp"

inline CycleDetectionResult undirected_cycle_certificate(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    return undirected_cycle_with_edges(vertex_count, edges);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_UNDIRECTED_CYCLE_CERTIFICATE_HPP_INCLUDED
