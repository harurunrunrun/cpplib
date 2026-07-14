#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "floyd_warshall.hpp"

struct ShortestPathCostQuery{
    int from;
    int to;
};

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
