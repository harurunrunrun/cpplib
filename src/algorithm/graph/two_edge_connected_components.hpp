#pragma once

#include <stdexcept>
#include <utility>
#include <vector>
#include "lowlink.hpp"

struct TwoEdgeConnectedComponentsResult{
    int count;
    std::vector<int> id;
    std::vector<std::vector<int>> groups;
    std::vector<int> bridges;
};

TwoEdgeConnectedComponentsResult two_edge_connected_components(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (two_edge_connected_components).");
    }
    auto low = lowlink(n, edges);
    std::vector<char> is_bridge(edges.size(), 0);
    for(int id: low.bridges) is_bridge[static_cast<std::size_t>(id)] = 1;

    std::vector<std::vector<std::pair<int, int>>> graph(static_cast<std::size_t>(n));
    for(int id = 0; id < static_cast<int>(edges.size()); id++){
        auto [u, v] = edges[static_cast<std::size_t>(id)];
        graph[static_cast<std::size_t>(u)].push_back({v, id});
        graph[static_cast<std::size_t>(v)].push_back({u, id});
    }

    TwoEdgeConnectedComponentsResult result;
    result.id.assign(static_cast<std::size_t>(n), -1);
    result.bridges = low.bridges;
    for(int s = 0; s < n; s++){
        if(result.id[static_cast<std::size_t>(s)] != -1) continue;
        int id = static_cast<int>(result.groups.size());
        result.groups.push_back({});
        std::vector<int> stack{s};
        result.id[static_cast<std::size_t>(s)] = id;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            result.groups[static_cast<std::size_t>(id)].push_back(v);
            for(auto [to, edge_id]: graph[static_cast<std::size_t>(v)]){
                if(is_bridge[static_cast<std::size_t>(edge_id)]) continue;
                if(result.id[static_cast<std::size_t>(to)] == -1){
                    result.id[static_cast<std::size_t>(to)] = id;
                    stack.push_back(to);
                }
            }
        }
    }
    result.count = static_cast<int>(result.groups.size());
    return result;
}
