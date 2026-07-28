#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_NAGAMOCHI_IBARAKI_SPARSIFICATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_NAGAMOCHI_IBARAKI_SPARSIFICATION_HPP_INCLUDED

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

inline std::vector<int> nagamochi_ibaraki_sparsification(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges,
    int connectivity
){
    if(vertex_count < 0 || connectivity < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(nagamochi_ibaraki_sparsification)."
        );
    }
    std::vector<int> remaining;
    remaining.reserve(edges.size());
    for(int edge_id = 0; edge_id < static_cast<int>(edges.size()); ++edge_id){
        auto [left, right] = edges[static_cast<std::size_t>(edge_id)];
        if(left < 0 || vertex_count <= left
            || right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(nagamochi_ibaraki_sparsification)."
            );
        }
        if(left != right) remaining.push_back(edge_id);
    }
    std::vector<int> selected;
    const std::size_t maximum_size =
        static_cast<std::size_t>(connectivity)
            * static_cast<std::size_t>(std::max(0, vertex_count - 1));
    selected.reserve(std::min(maximum_size, remaining.size()));
    std::vector<int> parent(static_cast<std::size_t>(vertex_count));
    std::vector<int> size(static_cast<std::size_t>(vertex_count));
    for(int round = 0; round < connectivity && !remaining.empty(); ++round){
        std::iota(parent.begin(), parent.end(), 0);
        std::fill(size.begin(), size.end(), 1);
        auto leader = [&](int vertex){
            int root = vertex;
            while(parent[static_cast<std::size_t>(root)] != root){
                root = parent[static_cast<std::size_t>(root)];
            }
            while(parent[static_cast<std::size_t>(vertex)] != vertex){
                int next = parent[static_cast<std::size_t>(vertex)];
                parent[static_cast<std::size_t>(vertex)] = root;
                vertex = next;
            }
            return root;
        };
        std::vector<int> next_remaining;
        next_remaining.reserve(remaining.size());
        for(int edge_id: remaining){
            auto [left, right] = edges[static_cast<std::size_t>(edge_id)];
            left = leader(left);
            right = leader(right);
            if(left == right){
                next_remaining.push_back(edge_id);
                continue;
            }
            if(size[static_cast<std::size_t>(left)]
                < size[static_cast<std::size_t>(right)]){
                std::swap(left, right);
            }
            parent[static_cast<std::size_t>(right)] = left;
            size[static_cast<std::size_t>(left)] +=
                size[static_cast<std::size_t>(right)];
            selected.push_back(edge_id);
        }
        remaining = std::move(next_remaining);
    }
    std::sort(selected.begin(), selected.end());
    return selected;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_NAGAMOCHI_IBARAKI_SPARSIFICATION_HPP_INCLUDED
