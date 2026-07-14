#pragma once

#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

#include "dijkstra.hpp"

template<class T>
struct OptionalShortestPathEdge{
    int from;
    int to;
    T cost;
};

template<class T>
std::optional<T> shortest_path_with_at_most_one_optional_edge(
    const std::vector<std::vector<DijkstraEdge<T>>>& graph,
    int source,
    int target,
    const std::vector<OptionalShortestPathEdge<T>>& optional_edges,
    bool optional_edges_are_bidirectional = false,
    T inf = std::numeric_limits<T>::max() / 4
){
    const int vertex_count = static_cast<int>(graph.size());
    if(source < 0 || vertex_count <= source ||
       target < 0 || vertex_count <= target)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(shortest_path_with_at_most_one_optional_edge)."
        );
    }
    for(const auto& edge: optional_edges){
        if(edge.from < 0 || vertex_count <= edge.from ||
           edge.to < 0 || vertex_count <= edge.to || edge.cost < T(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(shortest_path_with_at_most_one_optional_edge)."
            );
        }
    }

    const DijkstraResult<T> from_source = dijkstra(graph, source, inf);

    std::vector<std::vector<DijkstraEdge<T>>> reversed(
        static_cast<std::size_t>(vertex_count)
    );
    for(int from = 0; from < vertex_count; ++from){
        for(const auto& edge: graph[static_cast<std::size_t>(from)]){
            reversed[static_cast<std::size_t>(edge.to)].push_back(
                {from, edge.cost}
            );
        }
    }
    const DijkstraResult<T> to_target = dijkstra(reversed, target, inf);

    std::optional<T> answer;
    if(from_source.reachable[static_cast<std::size_t>(target)]){
        answer = from_source.dist[static_cast<std::size_t>(target)];
    }

    auto use_optional_edge = [&](int from, int to, T cost){
        if(!from_source.reachable[static_cast<std::size_t>(from)] ||
           !to_target.reachable[static_cast<std::size_t>(to)]) return;
        const T candidate =
            from_source.dist[static_cast<std::size_t>(from)] + cost +
            to_target.dist[static_cast<std::size_t>(to)];
        if(!answer || candidate < *answer) answer = candidate;
    };

    for(const auto& edge: optional_edges){
        use_optional_edge(edge.from, edge.to, edge.cost);
        if(optional_edges_are_bidirectional){
            use_optional_edge(edge.to, edge.from, edge.cost);
        }
    }
    return answer;
}
