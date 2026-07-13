#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "detail/undirected_graph.hpp"

namespace approximate::graph {

struct VertexCoverResult {
    std::vector<std::size_t> vertices;
    std::vector<std::size_t> matching_edge_indices;
};

[[nodiscard]] inline VertexCoverResult maximal_matching_vertex_cover(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    for(const auto [left, right] : edges){
        detail::validate_endpoint(vertex_count, left);
        detail::validate_endpoint(vertex_count, right);
    }
    std::vector<bool> selected(vertex_count, false);
    std::vector<bool> matched(vertex_count, false);
    for(const auto [left, right] : edges){
        if(left == right){
            selected[left] = true;
            matched[left] = true;
        }
    }

    VertexCoverResult result;
    for(std::size_t edge_index = 0; edge_index < edges.size(); ++edge_index){
        const auto [left, right] = edges[edge_index];
        if(left == right || selected[left] || selected[right]) continue;
        if(!matched[left] && !matched[right]){
            matched[left] = true;
            matched[right] = true;
            selected[left] = true;
            selected[right] = true;
            result.matching_edge_indices.push_back(edge_index);
        }
    }
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        if(selected[vertex]) result.vertices.push_back(vertex);
    }
    return result;
}

[[nodiscard]] inline VertexCoverResult two_approximate_vertex_cover(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    return maximal_matching_vertex_cover(vertex_count, edges);
}

}  // namespace approximate::graph
