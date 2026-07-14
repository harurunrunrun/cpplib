#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "dijkstra.hpp"

template<class Cost>
Cost minimum_forward_or_jump_cost(
    const std::vector<Cost>& forward_cost,
    const std::vector<Cost>& jump_cost,
    const std::vector<int>& jump_target
){
    if(forward_cost.size() != jump_cost.size() ||
       forward_cost.size() != jump_target.size() ||
       forward_cost.size() >=
           static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::runtime_error(
            "library assertion fault: size mismatch "
            "(minimum_forward_or_jump_cost)."
        );
    }

    const int vertex_count = static_cast<int>(forward_cost.size()) + 1;
    std::vector<std::vector<DijkstraEdge<Cost>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(int vertex = 0; vertex + 1 < vertex_count; ++vertex){
        const std::size_t index = static_cast<std::size_t>(vertex);
        if(forward_cost[index] < Cost{} || jump_cost[index] < Cost{} ||
           jump_target[index] < 0 || vertex_count <= jump_target[index]){
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(minimum_forward_or_jump_cost)."
            );
        }
        graph[index].push_back({vertex + 1, forward_cost[index]});
        graph[index].push_back({jump_target[index], jump_cost[index]});
    }
    return dijkstra(graph, 0).dist[static_cast<std::size_t>(vertex_count - 1)];
}
