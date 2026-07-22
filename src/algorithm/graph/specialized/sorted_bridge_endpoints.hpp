#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_SORTED_BRIDGE_ENDPOINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_SORTED_BRIDGE_ENDPOINTS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "../connectivity/lowlink.hpp"

inline std::vector<std::pair<int, int>> sorted_bridge_endpoints(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    const auto result = lowlink(vertex_count, edges);
    std::vector<std::pair<int, int>> answer;
    answer.reserve(result.bridges.size());
    for(const int edge: result.bridges){
        auto [left, right] = edges[static_cast<std::size_t>(edge)];
        if(right < left) std::swap(left, right);
        answer.emplace_back(left, right);
    }
    std::sort(answer.begin(), answer.end());
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_SORTED_BRIDGE_ENDPOINTS_HPP_INCLUDED
