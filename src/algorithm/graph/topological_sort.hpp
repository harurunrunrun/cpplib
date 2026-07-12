#pragma once

#include <queue>
#include <stdexcept>
#include <vector>

struct TopologicalSortResult{
    std::vector<int> order;
    bool is_dag;
};

TopologicalSortResult topological_sort(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    std::vector<int> indegree(static_cast<std::size_t>(n), 0);
    for(const auto& edges: graph){
        for(int to: edges){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (topological_sort).");
            }
            indegree[static_cast<std::size_t>(to)]++;
        }
    }

    std::queue<int> que;
    for(int v = 0; v < n; v++){
        if(indegree[static_cast<std::size_t>(v)] == 0) que.push(v);
    }
    TopologicalSortResult result;
    result.order.reserve(static_cast<std::size_t>(n));
    while(!que.empty()){
        int v = que.front();
        que.pop();
        result.order.push_back(v);
        for(int to: graph[static_cast<std::size_t>(v)]){
            indegree[static_cast<std::size_t>(to)]--;
            if(indegree[static_cast<std::size_t>(to)] == 0) que.push(to);
        }
    }
    result.is_dag = static_cast<int>(result.order.size()) == n;
    return result;
}
