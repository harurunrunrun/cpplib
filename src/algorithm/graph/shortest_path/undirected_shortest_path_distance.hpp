#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_UNDIRECTED_SHORTEST_PATH_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_UNDIRECTED_SHORTEST_PATH_DISTANCE_HPP_INCLUDED

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

#include "dijkstra.hpp"

template<class T>
struct UndirectedShortestPathEdge{
    int left;
    int right;
    T cost;
};

template<class T>
std::optional<T> undirected_shortest_path_distance(
    int vertex_count,
    const std::vector<UndirectedShortestPathEdge<T>>& edges,
    int source,
    int target
){
    if(vertex_count <= 0 || source < 0 || vertex_count <= source ||
       target < 0 || vertex_count <= target)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(undirected_shortest_path_distance)."
        );
    }
    std::vector<std::vector<DijkstraEdge<T>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto& edge: edges){
        if(edge.left < 0 || vertex_count <= edge.left ||
           edge.right < 0 || vertex_count <= edge.right ||
           edge.cost < T(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge violation "
                "(undirected_shortest_path_distance)."
            );
        }
        graph[static_cast<std::size_t>(edge.left)].push_back(
            {edge.right, edge.cost}
        );
        graph[static_cast<std::size_t>(edge.right)].push_back(
            {edge.left, edge.cost}
        );
    }
    const auto shortest = dijkstra<T>(graph, source);
    if(!shortest.reachable[static_cast<std::size_t>(target)]) return std::nullopt;
    return shortest.dist[static_cast<std::size_t>(target)];
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_UNDIRECTED_SHORTEST_PATH_DISTANCE_HPP_INCLUDED
