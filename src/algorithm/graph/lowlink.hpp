#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

struct LowlinkResult{
    std::vector<int> ord;
    std::vector<int> low;
    std::vector<int> articulation;
    std::vector<int> bridges;
};

LowlinkResult lowlink(int n, const std::vector<std::pair<int, int>>& edges){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (lowlink).");
    }
    std::vector<std::vector<std::pair<int, int>>> graph(static_cast<std::size_t>(n));
    for(int id = 0; id < static_cast<int>(edges.size()); id++){
        auto [u, v] = edges[static_cast<std::size_t>(id)];
        if(u < 0 || n <= u || v < 0 || n <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (lowlink).");
        }
        graph[static_cast<std::size_t>(u)].push_back({v, id});
        graph[static_cast<std::size_t>(v)].push_back({u, id});
    }

    LowlinkResult result;
    result.ord.assign(static_cast<std::size_t>(n), -1);
    result.low.assign(static_cast<std::size_t>(n), -1);
    int timer = 0;

    auto dfs = [&](auto&& self, int v, int parent_edge) -> void {
        result.ord[static_cast<std::size_t>(v)] = result.low[static_cast<std::size_t>(v)] = timer++;
        int child_count = 0;
        bool articulation = false;
        for(auto [to, id]: graph[static_cast<std::size_t>(v)]){
            if(id == parent_edge) continue;
            if(result.ord[static_cast<std::size_t>(to)] == -1){
                child_count++;
                self(self, to, id);
                result.low[static_cast<std::size_t>(v)] = std::min(
                    result.low[static_cast<std::size_t>(v)],
                    result.low[static_cast<std::size_t>(to)]
                );
                if(parent_edge != -1 &&
                   result.ord[static_cast<std::size_t>(v)] <= result.low[static_cast<std::size_t>(to)]){
                    articulation = true;
                }
                if(result.ord[static_cast<std::size_t>(v)] < result.low[static_cast<std::size_t>(to)]){
                    result.bridges.push_back(id);
                }
            }else{
                result.low[static_cast<std::size_t>(v)] = std::min(
                    result.low[static_cast<std::size_t>(v)],
                    result.ord[static_cast<std::size_t>(to)]
                );
            }
        }
        if(parent_edge == -1 && child_count >= 2) articulation = true;
        if(articulation) result.articulation.push_back(v);
    };

    for(int v = 0; v < n; v++){
        if(result.ord[static_cast<std::size_t>(v)] == -1){
            dfs(dfs, v, -1);
        }
    }
    std::sort(result.articulation.begin(), result.articulation.end());
    std::sort(result.bridges.begin(), result.bridges.end());
    return result;
}
