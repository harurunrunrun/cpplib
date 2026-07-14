#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "lowlink.hpp"

inline std::vector<std::pair<int, int>> critical_edges(
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
