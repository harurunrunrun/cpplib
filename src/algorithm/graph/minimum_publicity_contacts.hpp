#pragma once

#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

inline int minimum_publicity_contacts(
    int peer_count,
    const std::vector<std::pair<int, int>>& friendships
){
    if(peer_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (minimum_publicity_contacts)."
        );
    }
    std::vector<int> parent(static_cast<std::size_t>(peer_count));
    std::vector<int> size(static_cast<std::size_t>(peer_count), 1);
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
    int component_count = peer_count;
    for(const auto& [left_value, right_value]: friendships){
        if(left_value < 0 || peer_count <= left_value ||
           right_value < 0 || peer_count <= right_value)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(minimum_publicity_contacts)."
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
