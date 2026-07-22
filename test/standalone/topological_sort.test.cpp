// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/traversal/topological_sort.hpp"

bool valid_order(const std::vector<std::vector<int>>& graph, const TopologicalSortResult& result){
    const int n = static_cast<int>(graph.size());
    if(!result.is_dag) return true;
    if(static_cast<int>(result.order.size()) != n) return false;
    std::vector<int> pos(static_cast<std::size_t>(n), -1);
    for(int i = 0; i < n; i++) pos[static_cast<std::size_t>(result.order[static_cast<std::size_t>(i)])] = i;
    for(int v = 0; v < n; v++){
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(pos[static_cast<std::size_t>(v)] >= pos[static_cast<std::size_t>(to)]) return false;
        }
    }
    return true;
}

void self_test(){
    {
        std::vector<std::vector<int>> graph(4);
        graph[0] = {1, 2};
        graph[1] = {3};
        graph[2] = {3};
        auto res = topological_sort(graph);
        assert(res.is_dag);
        assert(valid_order(graph, res));
    }
    {
        // AOJ GRL_4_B publishes the ascending-start, input-edge-order DFS
        // reverse postorder as its deterministic cached answer.
        std::vector<std::vector<int>> graph(6);
        graph[0] = {1};
        graph[1] = {2};
        graph[3] = {1, 4};
        graph[4] = {5};
        graph[5] = {2};
        const auto res = topological_sort(graph);
        assert(res.is_dag);
        assert((res.order == std::vector<int>{3, 4, 5, 0, 1, 2}));
    }
    {
        std::vector<std::vector<int>> graph(2);
        graph[0] = {1};
        graph[1] = {0};
        auto res = topological_sort(graph);
        assert(!res.is_dag);
        assert(res.order.size() == graph.size());
    }
    {
        std::vector<std::vector<int>> graph(8);
        graph[0] = {3, 1};
        graph[2] = {1, 4};
        graph[3] = {1, 2};
        graph[4] = {1};
        graph[7] = {1, 5};
        const auto res = lexicographical_topological_sort(graph);
        assert(res.is_dag);
        assert((res.order == std::vector<int>{0, 3, 2, 4, 6, 7, 1, 5}));
    }
    {
        std::vector<std::vector<int>> graph(3);
        graph[0] = {1};
        graph[1] = {2};
        graph[2] = {0};
        const auto res = lexicographical_topological_sort(graph);
        assert(!res.is_dag);
        assert(res.order.empty());
    }
    for(int n = 1; n <= 50; n++){
        std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
        for(int u = 0; u < n; u++){
            for(int v = u + 1; v < n; v++){
                if((u * 31 + v * 17) % 7 == 0) graph[static_cast<std::size_t>(u)].push_back(v);
            }
        }
        auto res = topological_sort(graph);
        assert(res.is_dag);
        assert(valid_order(graph, res));
    }
}

int main(){
    self_test();
    int n, m;
    if(!(std::cin >> n >> m)){
        return 0;
    }
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        graph[static_cast<std::size_t>(u)].push_back(v);
    }
    auto res = topological_sort(graph);
    std::cout << static_cast<int>(res.is_dag) << '\n';
}
