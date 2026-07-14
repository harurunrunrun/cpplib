#pragma once

#include <cstddef>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

inline bool has_exactly_once_guard_coverage(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges,
    const std::vector<std::pair<int, int>>& guards
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::invalid_argument("guard coverage negative vertex count");
    }
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(vertex_count));
    for(const auto [left, right]: edges){
        if(left < 0 || vertex_count <= left || right < 0 ||
           vertex_count <= right)[[unlikely]]{
            throw std::out_of_range("guard coverage edge out of range");
        }
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }
    std::vector<int> owner(static_cast<std::size_t>(vertex_count), -1);
    std::queue<std::tuple<int, int, int>> queue;
    for(int guard = 0; guard < static_cast<int>(guards.size()); ++guard){
        const auto [base, strength] = guards[static_cast<std::size_t>(guard)];
        if(base < 0 || vertex_count <= base)[[unlikely]]{
            throw std::out_of_range("guard coverage base out of range");
        }
        if(strength < 0)[[unlikely]]{
            throw std::invalid_argument("guard coverage negative strength");
        }
        if(owner[static_cast<std::size_t>(base)] != -1) return false;
        owner[static_cast<std::size_t>(base)] = guard;
        queue.emplace(base, strength, guard);
    }
    while(!queue.empty()){
        const auto [vertex, remaining, guard] = queue.front();
        queue.pop();
        if(remaining == 0) continue;
        for(const int to: graph[static_cast<std::size_t>(vertex)]){
            int& protected_by = owner[static_cast<std::size_t>(to)];
            if(protected_by == guard) continue;
            if(protected_by != -1) return false;
            protected_by = guard;
            queue.emplace(to, remaining - 1, guard);
        }
    }
    for(const int protected_by: owner){
        if(protected_by == -1) return false;
    }
    return true;
}
