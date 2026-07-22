#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_ALMOST_SHORTEST_PATH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_ALMOST_SHORTEST_PATH_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <vector>

#include "dijkstra.hpp"
#include "shortest_path_edge_usage.hpp"

template<class T>
using AlmostShortestPathEdge = ShortestPathEdgeUsageEdge<T>;

template<class T>
struct AlmostShortestPathResult{
    T distance;
    bool reachable;
    std::vector<char> removed;
};

template<class T>
AlmostShortestPathResult<T> almost_shortest_path(
    int n,
    const std::vector<AlmostShortestPathEdge<T>>& edges,
    int source,
    int target,
    T inf = std::numeric_limits<T>::max() / 4
){
    auto usage = shortest_path_edge_usage(n, edges, source, target, inf);

    AlmostShortestPathResult<T> result;
    result.distance = inf;
    result.reachable = false;
    result.removed = std::move(usage.used);

    if(source == target){
        std::fill(result.removed.begin(), result.removed.end(), 0);
        result.distance = T(0);
        result.reachable = true;
        return result;
    }
    if(!usage.reachable) return result;

    std::vector<std::vector<DijkstraEdge<T>>> graph(
        static_cast<std::size_t>(n)
    );
    for(std::size_t edge_index = 0; edge_index < edges.size(); edge_index++){
        if(result.removed[edge_index]) continue;
        const auto& edge = edges[edge_index];
        graph[static_cast<std::size_t>(edge.from)].push_back({
            edge.to, edge.cost
        });
    }

    auto reduced = dijkstra(graph, source, inf);
    if(reduced.reachable[static_cast<std::size_t>(target)]){
        result.distance = reduced.dist[static_cast<std::size_t>(target)];
        result.reachable = true;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_ALMOST_SHORTEST_PATH_HPP_INCLUDED
