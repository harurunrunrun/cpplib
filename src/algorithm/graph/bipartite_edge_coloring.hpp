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

    std::vector<char> seen_left(static_cast<std::size_t>(left_size));
    std::vector<char> seen_right(static_cast<std::size_t>(right_size));
    std::vector<std::pair<int, int>> stack;
    std::vector<int> component_edges;

    for(int id = 0; id < static_cast<int>(edges.size()); id++){
        const auto& e = edges[static_cast<std::size_t>(id)];
        int a = missing_left(e.left);
        int b = missing_right(e.right);
        if(a == b){
            set_color(id, a);
            continue;
        }

        std::fill(seen_left.begin(), seen_left.end(), 0);
        std::fill(seen_right.begin(), seen_right.end(), 0);
        stack.clear();
        component_edges.clear();
        seen_left[static_cast<std::size_t>(e.left)] = 1;
        stack.push_back({0, e.left});

        while(!stack.empty()){
            auto [side, v] = stack.back();
            stack.pop_back();
            for(int c: {a, b}){
                int edge_id;
                if(side == 0){
                    edge_id = left_color[static_cast<std::size_t>(v)][static_cast<std::size_t>(c)];
                }else{
                    edge_id = right_color[static_cast<std::size_t>(v)][static_cast<std::size_t>(c)];
                }
                if(edge_id == -1) continue;
                component_edges.push_back(edge_id);
                const auto& edge = edges[static_cast<std::size_t>(edge_id)];
                if(side == 0){
                    if(!seen_right[static_cast<std::size_t>(edge.right)]){
                        seen_right[static_cast<std::size_t>(edge.right)] = 1;
                        stack.push_back({1, edge.right});
                    }
                }else{
                    if(!seen_left[static_cast<std::size_t>(edge.left)]){
                        seen_left[static_cast<std::size_t>(edge.left)] = 1;
                        stack.push_back({0, edge.left});
                    }
                }
            }
        }

        std::sort(component_edges.begin(), component_edges.end());
        component_edges.erase(std::unique(component_edges.begin(), component_edges.end()), component_edges.end());
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
