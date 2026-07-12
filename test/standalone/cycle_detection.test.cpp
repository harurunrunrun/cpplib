// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/cycle_detection.hpp"

bool has_directed_cycle_brute(const std::vector<std::vector<int>>& graph){
    return !directed_cycle(graph).empty();
}

void self_test(){
    {
        std::vector<std::vector<int>> graph(3);
        graph[0].push_back(1);
        graph[1].push_back(2);
        graph[2].push_back(0);
        auto cycle = directed_cycle(graph);
        assert(!cycle.empty());
    }
    {
        std::vector<std::pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 0}};
        auto cycle = undirected_cycle(3, edges);
        assert(!cycle.empty());
    }
    {
        std::vector<std::pair<int, int>> edges = {{0, 1}, {0, 1}};
        auto cycle = undirected_cycle(2, edges);
        assert(cycle.size() == 2);
    }
    for(int n = 1; n <= 30; n++){
        std::vector<std::vector<int>> dag(static_cast<std::size_t>(n));
        for(int u = 0; u < n; u++){
            for(int v = u + 1; v < n; v++){
                if((u * 17 + v * 31) % 5 == 0) dag[static_cast<std::size_t>(u)].push_back(v);
            }
        }
        assert(directed_cycle(dag).empty());
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        graph[static_cast<std::size_t>(u)].push_back(v);
    }
    std::cout << static_cast<int>(has_directed_cycle_brute(graph)) << '\n';
}
