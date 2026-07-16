#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "dijkstra.hpp"
#include "floyd_warshall.hpp"

struct ShortestPathCostQuery{
    int from;
    int to;
};

template<class T>
struct ShortestPathCostEdge{
    int from;
    int to;
    T cost;
};

namespace sum_shortest_path_query_costs_internal{

inline std::vector<std::vector<int>> group_queries_by_source(
    int vertex_count,
    const std::vector<ShortestPathCostQuery>& queries
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative vertex count "
            "(sum_shortest_path_query_costs)."
        );
    }
    std::vector<std::vector<int>> grouped(
        static_cast<std::size_t>(vertex_count)
    );
    for(int index = 0; index < static_cast<int>(queries.size()); ++index){
        const auto& query = queries[static_cast<std::size_t>(index)];
        if(query.from < 0 || vertex_count <= query.from ||
           query.to < 0 || vertex_count <= query.to)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: query violation "
                "(sum_shortest_path_query_costs)."
            );
        }
        grouped[static_cast<std::size_t>(query.from)].push_back(index);
    }
    return grouped;
}

template<class T>
std::vector<std::vector<DijkstraEdge<T>>> build_graph(
    int vertex_count,
    const std::vector<ShortestPathCostEdge<T>>& edges,
    bool require_nonnegative
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative vertex count "
            "(sum_shortest_path_query_costs)."
        );
    }
    std::vector<std::vector<DijkstraEdge<T>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from ||
           edge.to < 0 || vertex_count <= edge.to ||
           (require_nonnegative && edge.cost < T(0)))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge violation "
                "(sum_shortest_path_query_costs)."
            );
        }
        graph[static_cast<std::size_t>(edge.from)].push_back(
            {edge.to, edge.cost}
        );
    }
    return graph;
}

template<class T, class RestoreDistance>
T sum_grouped_dijkstra_queries(
    const std::vector<std::vector<DijkstraEdge<T>>>& graph,
    const std::vector<ShortestPathCostQuery>& queries,
    const std::vector<std::vector<int>>& grouped,
    const T& inf,
    RestoreDistance restore_distance
){
    T answer = T(0);
    for(int source = 0; source < static_cast<int>(graph.size()); ++source){
        if(grouped[static_cast<std::size_t>(source)].empty()) continue;
        const auto shortest = dijkstra<T>(graph, source, inf);
        for(const int index: grouped[static_cast<std::size_t>(source)]){
            const int target = queries[static_cast<std::size_t>(index)].to;
            if(!shortest.reachable[static_cast<std::size_t>(target)])[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: unreachable query "
                    "(sum_shortest_path_query_costs)."
                );
            }
            answer += restore_distance(
                source,
                target,
                shortest.dist[static_cast<std::size_t>(target)]
            );
        }
    }
    return answer;
}

} // namespace sum_shortest_path_query_costs_internal

template<class T>
T sum_shortest_path_query_costs(
    std::vector<std::vector<T>> distance,
    const std::vector<ShortestPathCostQuery>& queries,
    const T& inf
){
    const auto shortest = floyd_warshall<T>(std::move(distance), inf);
    const int vertex_count = static_cast<int>(shortest.dist.size());
    T answer = T(0);
    for(const auto& query: queries){
        if(query.from < 0 || vertex_count <= query.from ||
           query.to < 0 || vertex_count <= query.to)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: query violation "
                "(sum_shortest_path_query_costs)."
            );
        }
        if(!shortest.reachable[static_cast<std::size_t>(query.from)]
                              [static_cast<std::size_t>(query.to)])[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: unreachable query "
                "(sum_shortest_path_query_costs)."
            );
        }
        if(shortest.has_negative_cycle){
            for(int middle = 0; middle < vertex_count; ++middle){
                if(shortest.negative[static_cast<std::size_t>(middle)] &&
                   shortest.reachable[static_cast<std::size_t>(query.from)]
                                     [static_cast<std::size_t>(middle)] &&
                   shortest.reachable[static_cast<std::size_t>(middle)]
                                     [static_cast<std::size_t>(query.to)])[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: unbounded query "
                        "(sum_shortest_path_query_costs)."
                    );
                }
            }
        }
        answer += shortest.dist[static_cast<std::size_t>(query.from)]
                               [static_cast<std::size_t>(query.to)];
    }
    return answer;
}

template<class T>
T sum_nonnegative_shortest_path_query_costs(
    int vertex_count,
    const std::vector<ShortestPathCostEdge<T>>& edges,
    const std::vector<ShortestPathCostQuery>& queries,
    const T& inf = std::numeric_limits<T>::max() / 4
){
    const auto grouped =
        sum_shortest_path_query_costs_internal::group_queries_by_source(
            vertex_count, queries
        );
    const auto graph = sum_shortest_path_query_costs_internal::build_graph(
        vertex_count, edges, true
    );
    return sum_shortest_path_query_costs_internal::
        sum_grouped_dijkstra_queries<T>(
            graph,
            queries,
            grouped,
            inf,
            [](int, int, const T& distance){ return distance; }
        );
}

template<class T>
T sum_shortest_path_query_costs_johnson(
    int vertex_count,
    const std::vector<ShortestPathCostEdge<T>>& edges,
    const std::vector<ShortestPathCostQuery>& queries,
    const T& inf = std::numeric_limits<T>::max() / 4
){
    const auto grouped =
        sum_shortest_path_query_costs_internal::group_queries_by_source(
            vertex_count, queries
        );
    (void)sum_shortest_path_query_costs_internal::build_graph(
        vertex_count, edges, false
    );

    std::vector<T> potential(static_cast<std::size_t>(vertex_count), T(0));
    for(int iteration = 0; iteration < vertex_count; ++iteration){
        bool changed = false;
        for(const auto& edge: edges){
            const T candidate =
                potential[static_cast<std::size_t>(edge.from)] + edge.cost;
            if(candidate < potential[static_cast<std::size_t>(edge.to)]){
                potential[static_cast<std::size_t>(edge.to)] = candidate;
                changed = true;
            }
        }
        if(!changed) break;
        if(iteration + 1 == vertex_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative cycle "
                "(sum_shortest_path_query_costs_johnson)."
            );
        }
    }

    std::vector<std::vector<DijkstraEdge<T>>> reweighted(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto& edge: edges){
        const T cost = edge.cost +
            potential[static_cast<std::size_t>(edge.from)] -
            potential[static_cast<std::size_t>(edge.to)];
        if(cost < T(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid reduced cost "
                "(sum_shortest_path_query_costs_johnson)."
            );
        }
        reweighted[static_cast<std::size_t>(edge.from)].push_back(
            {edge.to, cost}
        );
    }
    return sum_shortest_path_query_costs_internal::
        sum_grouped_dijkstra_queries<T>(
            reweighted,
            queries,
            grouped,
            inf,
            [&potential](int source, int target, const T& distance){
                return distance - potential[static_cast<std::size_t>(source)] +
                    potential[static_cast<std::size_t>(target)];
            }
        );
}
