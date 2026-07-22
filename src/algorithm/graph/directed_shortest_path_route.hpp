#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_DIRECTED_SHORTEST_PATH_ROUTE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_DIRECTED_SHORTEST_PATH_ROUTE_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "dijkstra.hpp"

template<class T>
struct DirectedShortestPathRouteEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct DirectedShortestPathRouteResult{
    bool reachable;
    T distance;
    std::vector<std::pair<int, int>> edges;
};

template<class T>
DirectedShortestPathRouteResult<T> directed_shortest_path_route(
    int vertex_count,
    const std::vector<DirectedShortestPathRouteEdge<T>>& edges,
    int source,
    int target
){
    if(vertex_count <= 0 || source < 0 || vertex_count <= source ||
       target < 0 || vertex_count <= target)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (directed_shortest_path_route)."
        );
    }

    std::vector<std::vector<DijkstraEdge<T>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from ||
           edge.to < 0 || vertex_count <= edge.to || edge.cost < T(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid edge (directed_shortest_path_route)."
            );
        }
        graph[static_cast<std::size_t>(edge.from)].push_back({edge.to, edge.cost});
    }

    const auto shortest = dijkstra(graph, source);
    DirectedShortestPathRouteResult<T> result{false, T(0), {}};
    if(!shortest.reachable[static_cast<std::size_t>(target)]) return result;

    result.reachable = true;
    result.distance = shortest.dist[static_cast<std::size_t>(target)];
    for(int vertex = target; vertex != source; ){
        const int parent = shortest.parent[static_cast<std::size_t>(vertex)];
        if(parent < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid parent (directed_shortest_path_route)."
            );
        }
        result.edges.emplace_back(parent, vertex);
        vertex = parent;
    }
    std::reverse(result.edges.begin(), result.edges.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_DIRECTED_SHORTEST_PATH_ROUTE_HPP_INCLUDED
