#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

struct TopologicalSortResult{
    std::vector<int> order;
    bool is_dag;
};

TopologicalSortResult topological_sort(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    for(const auto& edges: graph){
        for(int to: edges){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (topological_sort).");
            }
        }
    }

    TopologicalSortResult result;
    result.order.reserve(static_cast<std::size_t>(n));
    result.is_dag = true;

    std::vector<unsigned char> state(static_cast<std::size_t>(n), 0);
    std::vector<std::pair<int, int>> stack;
    stack.reserve(static_cast<std::size_t>(n));
    for(int start = 0; start < n; start++){
        if(state[static_cast<std::size_t>(start)] != 0) continue;
        state[static_cast<std::size_t>(start)] = 1;
        stack.emplace_back(start, 0);

        while(!stack.empty()){
            int vertex = stack.back().first;
            int& next_edge = stack.back().second;
            const auto& edges = graph[static_cast<std::size_t>(vertex)];
            if(next_edge == static_cast<int>(edges.size())){
                state[static_cast<std::size_t>(vertex)] = 2;
                result.order.push_back(vertex);
                stack.pop_back();
                continue;
            }

            int to = edges[static_cast<std::size_t>(next_edge++)];
            unsigned char to_state = state[static_cast<std::size_t>(to)];
            if(to_state == 0){
                state[static_cast<std::size_t>(to)] = 1;
                stack.emplace_back(to, 0);
            }else if(to_state == 1){
                result.is_dag = false;
            }
        }
    }

    std::reverse(result.order.begin(), result.order.end());
    return result;
}
