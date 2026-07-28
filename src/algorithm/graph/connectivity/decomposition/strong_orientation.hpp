#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_STRONG_ORIENTATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_STRONG_ORIENTATION_HPP_INCLUDED

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

inline std::optional<std::vector<std::pair<int, int>>> strong_orientation(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (strong_orientation)."
        );
    }
    const int edge_count = static_cast<int>(edges.size());
    std::vector<std::vector<std::pair<int, int>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(int edge_id = 0; edge_id < edge_count; ++edge_id){
        auto [u, v] = edges[static_cast<std::size_t>(edge_id)];
        if(u < 0 || vertex_count <= u || v < 0 || vertex_count <= v)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (strong_orientation)."
            );
        }
        graph[static_cast<std::size_t>(u)].emplace_back(v, edge_id);
        if(u != v){
            graph[static_cast<std::size_t>(v)].emplace_back(u, edge_id);
        }
    }
    if(vertex_count == 0){
        return std::vector<std::pair<int, int>>{};
    }

    std::vector<int> discovery(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> low(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> parent(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> parent_edge(static_cast<std::size_t>(vertex_count), -1);
    std::vector<std::size_t> next_edge(static_cast<std::size_t>(vertex_count), 0);
    std::vector<unsigned char> assigned(static_cast<std::size_t>(edge_count), 0);
    std::vector<std::pair<int, int>> orientation(
        static_cast<std::size_t>(edge_count)
    );
    std::vector<int> stack;
    stack.reserve(static_cast<std::size_t>(vertex_count));
    int timer = 0;
    bool has_bridge = false;
    discovery[0] = low[0] = timer++;
    stack.push_back(0);
    while(!stack.empty()){
        int vertex = stack.back();
        auto& cursor = next_edge[static_cast<std::size_t>(vertex)];
        if(cursor == graph[static_cast<std::size_t>(vertex)].size()){
            stack.pop_back();
            int edge_id = parent_edge[static_cast<std::size_t>(vertex)];
            if(edge_id != -1){
                int from = parent[static_cast<std::size_t>(vertex)];
                low[static_cast<std::size_t>(from)] = std::min(
                    low[static_cast<std::size_t>(from)],
                    low[static_cast<std::size_t>(vertex)]
                );
                if(low[static_cast<std::size_t>(vertex)]
                    > discovery[static_cast<std::size_t>(from)]){
                    has_bridge = true;
                }
            }
            continue;
        }
        auto [to, edge_id] =
            graph[static_cast<std::size_t>(vertex)][cursor++];
        if(assigned[static_cast<std::size_t>(edge_id)] != 0) continue;
        assigned[static_cast<std::size_t>(edge_id)] = 1;
        orientation[static_cast<std::size_t>(edge_id)] = {vertex, to};
        if(discovery[static_cast<std::size_t>(to)] == -1){
            parent[static_cast<std::size_t>(to)] = vertex;
            parent_edge[static_cast<std::size_t>(to)] = edge_id;
            discovery[static_cast<std::size_t>(to)] =
                low[static_cast<std::size_t>(to)] = timer++;
            stack.push_back(to);
        }else{
            low[static_cast<std::size_t>(vertex)] = std::min(
                low[static_cast<std::size_t>(vertex)],
                discovery[static_cast<std::size_t>(to)]
            );
        }
    }
    if(has_bridge) return std::nullopt;
    for(int value: discovery){
        if(value == -1) return std::nullopt;
    }
    return orientation;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_STRONG_ORIENTATION_HPP_INCLUDED
