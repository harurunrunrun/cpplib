#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_CACTUS_DECOMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_CACTUS_DECOMPOSITION_HPP_INCLUDED

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "two_vertex_connected_components.hpp"

struct CactusDecompositionResult{
    std::vector<int> bridge_edge_ids;
    std::vector<std::vector<int>> cycles;
    std::vector<std::vector<int>> cycle_edge_ids;
};

inline std::optional<CactusDecompositionResult> cactus_decomposition(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (cactus_decomposition)."
        );
    }
    for(auto [from, to]: edges){
        if(from < 0 || vertex_count <= from
            || to < 0 || vertex_count <= to)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(cactus_decomposition)."
            );
        }
        if(from == to) return std::nullopt;
    }
    TwoVertexConnectedComponentsResult components =
        two_vertex_connected_components(vertex_count, edges);
    CactusDecompositionResult result;
    for(std::size_t index = 0; index < components.edge_groups.size(); ++index){
        const auto& edge_group = components.edge_groups[index];
        const auto& vertex_group = components.vertex_groups[index];
        if(edge_group.empty()) continue;
        if(edge_group.size() == 1){
            result.bridge_edge_ids.push_back(edge_group[0]);
            continue;
        }
        if(edge_group.size() != vertex_group.size()) return std::nullopt;
        std::vector<std::vector<std::pair<int, int>>> graph(
            static_cast<std::size_t>(vertex_count)
        );
        for(int edge_id: edge_group){
            auto [from, to] = edges[static_cast<std::size_t>(edge_id)];
            graph[static_cast<std::size_t>(from)].emplace_back(to, edge_id);
            graph[static_cast<std::size_t>(to)].emplace_back(from, edge_id);
        }
        for(int vertex: vertex_group){
            if(graph[static_cast<std::size_t>(vertex)].size() != 2){
                return std::nullopt;
            }
            std::sort(
                graph[static_cast<std::size_t>(vertex)].begin(),
                graph[static_cast<std::size_t>(vertex)].end()
            );
        }
        int start = *std::min_element(vertex_group.begin(), vertex_group.end());
        int vertex = start;
        int previous_edge = -1;
        std::vector<int> cycle;
        std::vector<int> cycle_edges;
        do{
            cycle.push_back(vertex);
            const auto& adjacent = graph[static_cast<std::size_t>(vertex)];
            auto next = adjacent[0].second == previous_edge
                ? adjacent[1]
                : adjacent[0];
            cycle_edges.push_back(next.second);
            previous_edge = next.second;
            vertex = next.first;
            if(cycle.size() > edge_group.size()) return std::nullopt;
        }while(vertex != start);
        if(cycle_edges.size() != edge_group.size()) return std::nullopt;
        result.cycles.push_back(std::move(cycle));
        result.cycle_edge_ids.push_back(std::move(cycle_edges));
    }
    std::sort(
        result.bridge_edge_ids.begin(), result.bridge_edge_ids.end()
    );
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_CACTUS_DECOMPOSITION_HPP_INCLUDED
