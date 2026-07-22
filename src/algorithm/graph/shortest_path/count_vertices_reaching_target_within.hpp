#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_COUNT_VERTICES_REACHING_TARGET_WITHIN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_COUNT_VERTICES_REACHING_TARGET_WITHIN_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "dijkstra.hpp"

template<class T>
struct ReachingTargetEdge{
    int from;
    int to;
    T cost;
};

template<class T>
int count_vertices_reaching_target_within(
    int vertex_count,
    const std::vector<ReachingTargetEdge<T>>& edges,
    int target,
    const T& limit
){
    if(vertex_count <= 0 || target < 0 || vertex_count <= target)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(count_vertices_reaching_target_within)."
        );
    }

    std::vector<std::vector<DijkstraEdge<T>>> reversed_graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from ||
           edge.to < 0 || vertex_count <= edge.to || edge.cost < T(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge violation "
                "(count_vertices_reaching_target_within)."
            );
        }
        reversed_graph[static_cast<std::size_t>(edge.to)].push_back(
            {edge.from, edge.cost}
        );
    }

    const auto shortest = dijkstra<T>(reversed_graph, target);
    int answer = 0;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(shortest.reachable[static_cast<std::size_t>(vertex)] &&
           shortest.dist[static_cast<std::size_t>(vertex)] <= limit){
            ++answer;
        }
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_COUNT_VERTICES_REACHING_TARGET_WITHIN_HPP_INCLUDED
