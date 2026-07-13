#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace approximate::graph::detail {

struct UndirectedGraph {
    std::vector<std::vector<std::size_t>> adjacency;
    std::vector<bool> self_loop;
};

inline void validate_endpoint(std::size_t vertex_count, std::size_t vertex){
    if(vertex >= vertex_count){
        throw std::out_of_range("edge endpoint is outside the graph");
    }
}

[[nodiscard]] inline UndirectedGraph make_undirected_graph(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    UndirectedGraph graph;
    graph.adjacency.resize(vertex_count);
    graph.self_loop.assign(vertex_count, false);
    for(const auto [left, right] : edges){
        validate_endpoint(vertex_count, left);
        validate_endpoint(vertex_count, right);
        if(left == right){
            graph.self_loop[left] = true;
        }else{
            graph.adjacency[left].push_back(right);
            graph.adjacency[right].push_back(left);
        }
    }
    for(auto& neighbors : graph.adjacency){
        std::sort(neighbors.begin(), neighbors.end());
        neighbors.erase(std::unique(neighbors.begin(), neighbors.end()), neighbors.end());
    }
    return graph;
}

}  // namespace approximate::graph::detail
