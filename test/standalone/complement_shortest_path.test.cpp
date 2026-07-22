// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <queue>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/shortest_path/complement_shortest_path.hpp"

std::vector<int> brute(int n, const std::vector<std::vector<int>>& graph, int s){
    std::vector<std::vector<char>> has(static_cast<std::size_t>(n), std::vector<char>(static_cast<std::size_t>(n), 0));
    for(int v = 0; v < n; v++){
        for(int to: graph[static_cast<std::size_t>(v)]){
            has[static_cast<std::size_t>(v)][static_cast<std::size_t>(to)] = 1;
        }
    }
    std::vector<int> dist(static_cast<std::size_t>(n), -1);
    std::queue<int> que;
    dist[static_cast<std::size_t>(s)] = 0;
    que.push(s);
    while(!que.empty()){
        int v = que.front();
        que.pop();
        for(int to = 0; to < n; to++){
            if(v == to || has[static_cast<std::size_t>(v)][static_cast<std::size_t>(to)]) continue;
            if(dist[static_cast<std::size_t>(to)] != -1) continue;
            dist[static_cast<std::size_t>(to)] = dist[static_cast<std::size_t>(v)] + 1;
            que.push(to);
        }
    }
    return dist;
}

void self_test(){
    {
        std::vector<std::vector<int>> graph(4);
        graph[0] = {1, 2};
        graph[1] = {2, 3};
        graph[2] = {3};
        auto res = complement_shortest_path(graph, 0);
        assert((res.dist == std::vector<int>{0, 2, 2, 1}));
        assert(res.parent[3] == 0);
    }
    std::mt19937 rng(20260728);
    for(int n = 1; n <= 24; n++){
        for(int step = 0; step < 200; step++){
            std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u != v && rng() % 3 == 0){
                        graph[static_cast<std::size_t>(u)].push_back(v);
                    }
                }
            }
            int s = static_cast<int>(rng() % n);
            auto res = complement_shortest_path(graph, s);
            assert(res.dist == brute(n, graph, s));
        }
    }
}

int main(){
    int n, m, s;
    if(!(std::cin >> n >> m >> s)){
        self_test();
        return 0;
    }
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        graph[static_cast<std::size_t>(u)].push_back(v);
    }
    auto res = complement_shortest_path(graph, s);
    for(int i = 0; i < n; i++){
        if(i) std::cout << ' ';
        std::cout << res.dist[static_cast<std::size_t>(i)];
    }
    std::cout << '\n';
}
