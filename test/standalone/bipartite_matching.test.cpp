// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/matching/bipartite_matching.hpp"

int brute(int left_size, int right_size, const std::vector<std::pair<int, int>>& edges){
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(left_size));
    for(auto [l, r]: edges) graph[static_cast<std::size_t>(l)].push_back(r);
    std::vector<char> used(static_cast<std::size_t>(right_size), 0);
    int best = 0;
    auto dfs = [&](auto&& self, int v, int count) -> void {
        if(v == left_size){
            best = std::max(best, count);
            return;
        }
        self(self, v + 1, count);
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(!used[static_cast<std::size_t>(to)]){
                used[static_cast<std::size_t>(to)] = 1;
                self(self, v + 1, count + 1);
                used[static_cast<std::size_t>(to)] = 0;
            }
        }
    };
    dfs(dfs, 0, 0);
    return best;
}

void self_test(){
    {
        BipartiteMatching graph(3, 3);
        graph.add_edge(0, 0);
        graph.add_edge(0, 1);
        graph.add_edge(1, 1);
        graph.add_edge(2, 1);
        graph.add_edge(2, 2);
        assert(graph.solve().size == 3);
    }
    std::mt19937 rng(20260822);
    for(int l = 0; l <= 9; l++){
        for(int r = 0; r <= 9; r++){
            for(int step = 0; step < 80; step++){
                BipartiteMatching graph(l, r);
                std::vector<std::pair<int, int>> edges;
                for(int i = 0; i < l; i++){
                    for(int j = 0; j < r; j++){
                        if(rng() % 3 == 0){
                            graph.add_edge(i, j);
                            edges.push_back({i, j});
                        }
                    }
                }
                assert(graph.solve().size == brute(l, r, edges));
            }
        }
    }
}

int main(){
    int l, r, m;
    if(!(std::cin >> l >> r >> m)){
        self_test();
        return 0;
    }
    BipartiteMatching graph(l, r);
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        graph.add_edge(u, v);
    }
    std::cout << graph.solve().size << '\n';
}
