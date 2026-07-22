#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_TOPOLOGICAL_SORT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_TOPOLOGICAL_SORT_HPP_INCLUDED

#include <algorithm>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct TopologicalSortResult{
    std::vector<int> order;
    bool is_dag;
};

namespace topological_sort_internal{

inline void validate_graph(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    for(const auto& edges: graph){
        for(int to: edges){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (topological_sort).");
            }
        }
    }
}

} // namespace topological_sort_internal

TopologicalSortResult topological_sort(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    topological_sort_internal::validate_graph(graph);

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

TopologicalSortResult lexicographical_topological_sort(
    const std::vector<std::vector<int>>& graph
){
    const int n = static_cast<int>(graph.size());
    topological_sort_internal::validate_graph(graph);

    std::vector<int> indegree(static_cast<std::size_t>(n));
    for(const auto& edges: graph){
        for(int to: edges) indegree[static_cast<std::size_t>(to)]++;
    }
    std::priority_queue<int, std::vector<int>, std::greater<int>> queue;
    for(int vertex = 0; vertex < n; vertex++){
        if(indegree[static_cast<std::size_t>(vertex)] == 0) queue.push(vertex);
    }

    TopologicalSortResult result;
    result.order.reserve(static_cast<std::size_t>(n));
    while(!queue.empty()){
        int vertex = queue.top();
        queue.pop();
        result.order.push_back(vertex);
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(--indegree[static_cast<std::size_t>(to)] == 0) queue.push(to);
        }
    }
    result.is_dag = static_cast<int>(result.order.size()) == n;
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_TOPOLOGICAL_SORT_HPP_INCLUDED
