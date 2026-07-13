#pragma once

#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

#include "detail/undirected_graph.hpp"

namespace approximate::graph {

namespace detail {

struct DominationEntry {
    std::size_t gain;
    std::size_t vertex;
};

struct DominationEntryLess {
    [[nodiscard]] bool operator()(const DominationEntry& left, const DominationEntry& right) const noexcept {
        if(left.gain != right.gain) return left.gain < right.gain;
        return left.vertex > right.vertex;
    }
};

}  // namespace detail

[[nodiscard]] inline std::vector<std::size_t> greedy_dominating_set(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    std::vector<std::size_t> gains(vertex_count);
    std::priority_queue<detail::DominationEntry, std::vector<detail::DominationEntry>, detail::DominationEntryLess> heap;
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        gains[vertex] = graph.adjacency[vertex].size() + 1;
        heap.push({gains[vertex], vertex});
    }
    std::vector<bool> dominated(vertex_count, false);
    std::vector<std::size_t> result;
    std::size_t dominated_count = 0;
    while(dominated_count < vertex_count){
        detail::DominationEntry entry{};
        do{
            entry = heap.top();
            heap.pop();
        }while(entry.gain != gains[entry.vertex]);
        result.push_back(entry.vertex);
        const auto dominate = [&](std::size_t newly_dominated){
            if(dominated[newly_dominated]) return;
            dominated[newly_dominated] = true;
            ++dominated_count;
            --gains[newly_dominated];
            heap.push({gains[newly_dominated], newly_dominated});
            for(const std::size_t candidate : graph.adjacency[newly_dominated]){
                --gains[candidate];
                heap.push({gains[candidate], candidate});
            }
        };
        dominate(entry.vertex);
        for(const std::size_t neighbor : graph.adjacency[entry.vertex]) dominate(neighbor);
    }
    return result;
}

}  // namespace approximate::graph
