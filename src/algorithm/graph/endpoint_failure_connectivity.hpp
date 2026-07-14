#pragma once

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "single_failure_connectivity.hpp"

template<int MAX_VERTICES, int MAX_EDGES>
struct EndpointFailureConnectivity{
private:
    static std::uint64_t edge_key(int first, int second){
        if(second < first) std::swap(first, second);
        return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(first)) << 32)
            | static_cast<std::uint32_t>(second);
    }

    SingleFailureConnectivity<MAX_VERTICES, MAX_EDGES> oracle;
    std::unordered_map<std::uint64_t, int> edge_ids;

public:
    EndpointFailureConnectivity(
        int vertex_count,
        const std::vector<std::pair<int, int>>& edges
    ): oracle(vertex_count, edges){
        edge_ids.reserve(edges.size() * 2 + 1);
        for(int id = 0; id < static_cast<int>(edges.size()); ++id){
            const auto [left, right] = edges[static_cast<std::size_t>(id)];
            if(!edge_ids.emplace(edge_key(left, right), id).second){
                throw std::runtime_error("library assertion fault: parallel edge (constructor).");
            }
        }
    }

    bool connected_without_edge(
        int first,
        int second,
        int edge_left,
        int edge_right
    ) const{
        const auto iterator = edge_ids.find(edge_key(edge_left, edge_right));
        if(iterator == edge_ids.end()) throw std::runtime_error("library assertion fault: edge not found (connected_without_edge).");
        return oracle.connected_without_edge(first, second, iterator->second);
    }

    bool connected_without_vertex(int first, int second, int removed) const{
        return oracle.connected_without_vertex(first, second, removed);
    }
};
