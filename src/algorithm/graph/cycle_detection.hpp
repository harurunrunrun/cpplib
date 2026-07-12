#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

std::vector<int> directed_cycle(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    for(const auto& edges: graph){
        for(int to: edges){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (directed_cycle).");
            }
        }
    }

    std::vector<char> color(static_cast<std::size_t>(n), 0);
    std::vector<int> parent(static_cast<std::size_t>(n), -1);
    std::vector<int> result;
    auto dfs = [&](auto&& self, int v) -> bool {
        color[static_cast<std::size_t>(v)] = 1;
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(color[static_cast<std::size_t>(to)] == 0){
                parent[static_cast<std::size_t>(to)] = v;
                if(self(self, to)) return true;
            }else if(color[static_cast<std::size_t>(to)] == 1){
                result.push_back(to);
                for(int x = v; x != to; x = parent[static_cast<std::size_t>(x)]){
                    result.push_back(x);
                }
                std::reverse(result.begin(), result.end());
                return true;
            }
        }
        color[static_cast<std::size_t>(v)] = 2;
        return false;
    };
    for(int v = 0; v < n; v++){
        if(color[static_cast<std::size_t>(v)] == 0 && dfs(dfs, v)) break;
    }
    return result;
}

std::vector<int> undirected_cycle(int n, const std::vector<std::pair<int, int>>& edges){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (undirected_cycle).");
    }
    std::vector<std::vector<std::pair<int, int>>> graph(static_cast<std::size_t>(n));
    for(int id = 0; id < static_cast<int>(edges.size()); id++){
        auto [u, v] = edges[static_cast<std::size_t>(id)];
        if(u < 0 || n <= u || v < 0 || n <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (undirected_cycle).");
        }
        graph[static_cast<std::size_t>(u)].push_back({v, id});
        graph[static_cast<std::size_t>(v)].push_back({u, id});
    }

    std::vector<char> color(static_cast<std::size_t>(n), 0);
    std::vector<int> parent(static_cast<std::size_t>(n), -1);
    std::vector<int> result;
    auto dfs = [&](auto&& self, int v, int parent_edge) -> bool {
        color[static_cast<std::size_t>(v)] = 1;
        for(auto [to, id]: graph[static_cast<std::size_t>(v)]){
            if(id == parent_edge) continue;
            if(color[static_cast<std::size_t>(to)] == 0){
                parent[static_cast<std::size_t>(to)] = v;
                if(self(self, to, id)) return true;
            }else if(color[static_cast<std::size_t>(to)] == 1){
                result.push_back(to);
                for(int x = v; x != to; x = parent[static_cast<std::size_t>(x)]){
                    result.push_back(x);
                }
                std::reverse(result.begin(), result.end());
                return true;
            }
        }
        color[static_cast<std::size_t>(v)] = 2;
        return false;
    };
    for(int v = 0; v < n; v++){
        if(color[static_cast<std::size_t>(v)] == 0 && dfs(dfs, v, -1)) break;
    }
    return result;
}
