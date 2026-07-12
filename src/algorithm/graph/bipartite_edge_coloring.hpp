#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

struct BipartiteEdgeColoringEdge{
    int left;
    int right;
};

struct BipartiteEdgeColoringResult{
    int color_count;
    std::vector<int> color;
};

BipartiteEdgeColoringResult bipartite_edge_coloring(
    int left_size,
    int right_size,
    const std::vector<BipartiteEdgeColoringEdge>& edges
){
    if(left_size < 0 || right_size < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (bipartite_edge_coloring).");
    }
    std::vector<int> left_degree(static_cast<std::size_t>(left_size), 0);
    std::vector<int> right_degree(static_cast<std::size_t>(right_size), 0);
    for(const auto& e: edges){
        if(e.left < 0 || left_size <= e.left || e.right < 0 || right_size <= e.right)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (bipartite_edge_coloring).");
        }
        left_degree[static_cast<std::size_t>(e.left)]++;
        right_degree[static_cast<std::size_t>(e.right)]++;
    }
    int delta = 0;
    for(int x: left_degree) delta = std::max(delta, x);
    for(int x: right_degree) delta = std::max(delta, x);

    BipartiteEdgeColoringResult result;
    result.color_count = delta;
    result.color.assign(edges.size(), -1);
    if(delta == 0) return result;

    std::vector<std::vector<int>> left_color(
        static_cast<std::size_t>(left_size),
        std::vector<int>(static_cast<std::size_t>(delta), -1)
    );
    std::vector<std::vector<int>> right_color(
        static_cast<std::size_t>(right_size),
        std::vector<int>(static_cast<std::size_t>(delta), -1)
    );

    auto missing_left = [&](int v) -> int {
        for(int c = 0; c < delta; c++){
            if(left_color[static_cast<std::size_t>(v)][static_cast<std::size_t>(c)] == -1) return c;
        }
        return -1;
    };
    auto missing_right = [&](int v) -> int {
        for(int c = 0; c < delta; c++){
            if(right_color[static_cast<std::size_t>(v)][static_cast<std::size_t>(c)] == -1) return c;
        }
        return -1;
    };
    auto set_color = [&](int id, int color) -> void {
        const auto& e = edges[static_cast<std::size_t>(id)];
        result.color[static_cast<std::size_t>(id)] = color;
        left_color[static_cast<std::size_t>(e.left)][static_cast<std::size_t>(color)] = id;
        right_color[static_cast<std::size_t>(e.right)][static_cast<std::size_t>(color)] = id;
    };
    auto erase_color = [&](int id, int color) -> void {
        const auto& e = edges[static_cast<std::size_t>(id)];
        left_color[static_cast<std::size_t>(e.left)][static_cast<std::size_t>(color)] = -1;
        right_color[static_cast<std::size_t>(e.right)][static_cast<std::size_t>(color)] = -1;
    };

    std::vector<int> component_edges;

    for(int id = 0; id < static_cast<int>(edges.size()); id++){
        const auto& e = edges[static_cast<std::size_t>(id)];
        int a = missing_left(e.left);
        int b = missing_right(e.right);
        if(a == b){
            set_color(id, a);
            continue;
        }

        component_edges.clear();
        int side = 0;
        int vertex = e.left;
        int color = b;
        while(true){
            const int edge_id = side == 0
                ? left_color[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(color)]
                : right_color[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(color)];
            if(edge_id == -1) break;
            component_edges.push_back(edge_id);
            const auto& edge = edges[static_cast<std::size_t>(edge_id)];
            if(side == 0){
                vertex = edge.right;
                side = 1;
            }else{
                vertex = edge.left;
                side = 0;
            }
            color = color == a ? b : a;
        }

        for(int edge_id: component_edges){
            erase_color(edge_id, result.color[static_cast<std::size_t>(edge_id)]);
        }
        for(int edge_id: component_edges){
            int old = result.color[static_cast<std::size_t>(edge_id)];
            set_color(edge_id, old == a ? b : a);
        }
        set_color(id, b);
    }
    return result;
}
