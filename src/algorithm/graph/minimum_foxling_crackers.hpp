#pragma once

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

inline long long minimum_foxling_crackers(
    long long foxling_count,
    const std::vector<std::pair<long long, long long>>& friendships
){
    if(foxling_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (minimum_foxling_crackers)."
        );
    }
    std::vector<long long> vertices;
    vertices.reserve(friendships.size() * 2);
    for(const auto& [left, right]: friendships){
        if(left < 0 || foxling_count <= left || right < 0 ||
           foxling_count <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(minimum_foxling_crackers)."
            );
        }
        vertices.push_back(left);
        vertices.push_back(right);
    }
    std::sort(vertices.begin(), vertices.end());
    vertices.erase(std::unique(vertices.begin(), vertices.end()), vertices.end());
    std::vector<int> parent(vertices.size());
    std::vector<int> size(vertices.size(), 1);
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
    long long component_count = foxling_count;
    for(const auto& [left_value, right_value]: friendships){
        int left = static_cast<int>(
            std::lower_bound(vertices.begin(), vertices.end(), left_value) -
            vertices.begin()
        );
        int right = static_cast<int>(
            std::lower_bound(vertices.begin(), vertices.end(), right_value) -
            vertices.begin()
        );
        left = leader(left);
        right = leader(right);
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
