#pragma once

#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

inline int undirected_component_count(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (undirected_component_count)."
        );
    }
    std::vector<int> parent(static_cast<std::size_t>(vertex_count));
    std::vector<int> size(static_cast<std::size_t>(vertex_count), 1);
    std::iota(parent.begin(), parent.end(), 0);
    auto leader = [&](int vertex){
        int root = vertex;
        while(parent[static_cast<std::size_t>(root)] != root){
            root = parent[static_cast<std::size_t>(root)];
        }
        while(parent[static_cast<std::size_t>(vertex)] != vertex){
            const int next = parent[static_cast<std::size_t>(vertex)];
            parent[static_cast<std::size_t>(vertex)] = root;
            vertex = next;
        }
        return root;
    };
    int component_count = vertex_count;
    for(const auto& [left_value, right_value]: edges){
        if(left_value < 0 || vertex_count <= left_value ||
           right_value < 0 || vertex_count <= right_value)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(undirected_component_count)."
            );
        }
        int left = leader(left_value);
        int right = leader(right_value);
        if(left == right) continue;
        if(size[static_cast<std::size_t>(left)] < size[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        parent[static_cast<std::size_t>(right)] = left;
        size[static_cast<std::size_t>(left)] += size[static_cast<std::size_t>(right)];
        component_count--;
    }
    return component_count;
}
