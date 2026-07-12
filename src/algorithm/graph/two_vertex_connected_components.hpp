#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>
#include "lowlink.hpp"

struct TwoVertexConnectedComponentsResult{
    int count;
    std::vector<int> articulation;
    std::vector<std::vector<int>> vertex_groups;
    std::vector<std::vector<int>> edge_groups;
};

TwoVertexConnectedComponentsResult two_vertex_connected_components(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (two_vertex_connected_components).");
    }
    for(auto [u, v]: edges){
        if(u < 0 || n <= u || v < 0 || n <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (two_vertex_connected_components).");
        }
    }

    std::vector<std::vector<std::pair<int, int>>> graph(static_cast<std::size_t>(n));
    std::vector<int> degree(static_cast<std::size_t>(n), 0);
    for(int id = 0; id < static_cast<int>(edges.size()); id++){
        auto [u, v] = edges[static_cast<std::size_t>(id)];
        graph[static_cast<std::size_t>(u)].push_back({v, id});
        graph[static_cast<std::size_t>(v)].push_back({u, id});
        degree[static_cast<std::size_t>(u)]++;
        degree[static_cast<std::size_t>(v)]++;
    }

    TwoVertexConnectedComponentsResult result;
    result.articulation = lowlink(n, edges).articulation;

    std::vector<int> ord(static_cast<std::size_t>(n), -1), low(static_cast<std::size_t>(n), -1);
    std::vector<int> stack;
    int timer = 0;

    auto add_component = [&](std::vector<int> edge_group) -> void {
        std::vector<int> vertices;
        for(int id: edge_group){
            auto [u, v] = edges[static_cast<std::size_t>(id)];
            vertices.push_back(u);
            vertices.push_back(v);
        }
        std::sort(vertices.begin(), vertices.end());
        vertices.erase(std::unique(vertices.begin(), vertices.end()), vertices.end());
        std::sort(edge_group.begin(), edge_group.end());
        result.vertex_groups.push_back(vertices);
        result.edge_groups.push_back(edge_group);
    };

    auto dfs = [&](auto&& self, int v, int parent_edge) -> void {
        ord[static_cast<std::size_t>(v)] = low[static_cast<std::size_t>(v)] = timer++;
        for(auto [to, id]: graph[static_cast<std::size_t>(v)]){
            if(id == parent_edge) continue;
            if(ord[static_cast<std::size_t>(to)] == -1){
                stack.push_back(id);
                self(self, to, id);
                low[static_cast<std::size_t>(v)] = std::min(low[static_cast<std::size_t>(v)], low[static_cast<std::size_t>(to)]);
                if(ord[static_cast<std::size_t>(v)] <= low[static_cast<std::size_t>(to)]){
                    std::vector<int> edge_group;
                    while(true){
                        int edge_id = stack.back();
                        stack.pop_back();
                        edge_group.push_back(edge_id);
                        if(edge_id == id) break;
                    }
                    add_component(edge_group);
                }
            }else if(ord[static_cast<std::size_t>(to)] < ord[static_cast<std::size_t>(v)]){
                stack.push_back(id);
                low[static_cast<std::size_t>(v)] = std::min(low[static_cast<std::size_t>(v)], ord[static_cast<std::size_t>(to)]);
            }
        }
    };

    for(int v = 0; v < n; v++){
        if(ord[static_cast<std::size_t>(v)] == -1){
            if(degree[static_cast<std::size_t>(v)] == 0){
                result.vertex_groups.push_back({v});
                result.edge_groups.push_back({});
                ord[static_cast<std::size_t>(v)] = low[static_cast<std::size_t>(v)] = timer++;
            }else{
                dfs(dfs, v, -1);
            }
        }
    }
    result.count = static_cast<int>(result.vertex_groups.size());
    return result;
}
