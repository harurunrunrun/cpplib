#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_IS_UNDIRECTED_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_IS_UNDIRECTED_TREE_HPP_INCLUDED

#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

inline bool is_undirected_tree(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (is_undirected_tree)."
        );
    }
    if(vertex_count == 0) return false;
    if(edges.size() != static_cast<std::size_t>(vertex_count - 1)){
        for(const auto& [left, right]: edges){
            if(left < 0 || vertex_count <= left || right < 0 || vertex_count <= right)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (is_undirected_tree)."
                );
            }
        }
        return false;
    }
    std::vector<int> parent(static_cast<std::size_t>(vertex_count));
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
    for(const auto& [left_value, right_value]: edges){
        if(left_value < 0 || vertex_count <= left_value ||
           right_value < 0 || vertex_count <= right_value)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (is_undirected_tree)."
            );
        }
        const int left = leader(left_value);
        const int right = leader(right_value);
        if(left == right) return false;
        parent[static_cast<std::size_t>(right)] = left;
    }
    return true;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_IS_UNDIRECTED_TREE_HPP_INCLUDED
