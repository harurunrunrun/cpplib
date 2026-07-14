#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "kruskal_mst.hpp"

struct MinimumNetworkConnectionEdge{
    int from;
    int to;
    long long cost;
};

inline long long minimum_network_connection_cost(
    int vertex_count,
    const std::vector<MinimumNetworkConnectionEdge>& edges
){
    std::vector<KruskalEdge<__int128_t>> converted;
    converted.reserve(edges.size());
    for(const auto& edge: edges){
        if(edge.cost < 0)[[unlikely]]{
            throw std::invalid_argument(
                "minimum_network_connection_cost negative cost"
            );
        }
        converted.push_back({edge.from, edge.to, edge.cost});
    }
    const auto result = kruskal_mst<__int128_t>(vertex_count, converted);
    if(!result.connected)[[unlikely]]{
        throw std::invalid_argument(
            "minimum_network_connection_cost disconnected graph"
        );
    }
    if(result.cost > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "minimum_network_connection_cost overflow"
        );
    }
    return static_cast<long long>(result.cost);
}
