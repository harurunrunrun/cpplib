// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/constraints/maximum_clique.hpp"

int brute_size(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<char>> adj(static_cast<std::size_t>(n), std::vector<char>(static_cast<std::size_t>(n), 0));
    for(int v = 0; v < n; v++){
        for(int to: graph[static_cast<std::size_t>(v)]) adj[static_cast<std::size_t>(v)][static_cast<std::size_t>(to)] = 1;
    }
    int best = 0;
    for(int mask = 0; mask < (1 << n); mask++){
        bool ok = true;
        int cnt = 0;
        for(int i = 0; i < n; i++){
            if(!(mask >> i & 1)) continue;
            cnt++;
            for(int j = i + 1; j < n; j++){
                if((mask >> j & 1) && !adj[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)]){
                    ok = false;
                }
            }
        }
        if(ok && best < cnt) best = cnt;
    }
    return best;
}

bool is_clique(const std::vector<std::vector<int>>& graph, const std::vector<int>& vertices){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<char>> adj(static_cast<std::size_t>(n), std::vector<char>(static_cast<std::size_t>(n), 0));
    for(int v = 0; v < n; v++){
        for(int to: graph[static_cast<std::size_t>(v)]) adj[static_cast<std::size_t>(v)][static_cast<std::size_t>(to)] = 1;
    }
    for(std::size_t i = 0; i < vertices.size(); i++){
        for(std::size_t j = i + 1; j < vertices.size(); j++){
            if(!adj[static_cast<std::size_t>(vertices[i])][static_cast<std::size_t>(vertices[j])]) return false;
        }
    }
    return true;
}

void self_test(){
    {
        std::vector<std::vector<int>> graph(5);
        auto add = [&](int u, int v){
            graph[static_cast<std::size_t>(u)].push_back(v);
            graph[static_cast<std::size_t>(v)].push_back(u);
        };
        add(0, 1);
        add(0, 2);
        add(1, 2);
        add(2, 3);
        add(3, 4);
        auto clique = maximum_clique(graph);
        assert(clique.size() == 3);
        assert(is_clique(graph, clique));
    }
    std::mt19937 rng(20260806);
    for(int n = 0; n <= 13; n++){
        for(int step = 0; step < 80; step++){
            std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 3 == 0){
                        graph[static_cast<std::size_t>(u)].push_back(v);
                        graph[static_cast<std::size_t>(v)].push_back(u);
                    }
                }
            }
            auto clique = maximum_clique(graph);
            assert(is_clique(graph, clique));
            assert(static_cast<int>(clique.size()) == brute_size(graph));
        }
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
        graph[static_cast<std::size_t>(v)].push_back(u);
    }
    auto clique = maximum_clique(graph);
    std::cout << clique.size() << '\n';
}
