// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc277/tasks/abc277_e

#include <algorithm>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/zero_one_bfs.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m, k;
    std::cin >> n >> m >> k;
    std::vector<std::vector<ZeroOneBFSEdge>> graph(static_cast<std::size_t>(2 * n));
    for(int i = 0; i < m; ++i){
        int u, v, state;
        std::cin >> u >> v >> state;
        --u;
        --v;
        u += state * n;
        v += state * n;
        graph[static_cast<std::size_t>(u)].push_back({v, 1});
        graph[static_cast<std::size_t>(v)].push_back({u, 1});
    }
    for(int i = 0; i < k; ++i){
        int v;
        std::cin >> v;
        --v;
        graph[static_cast<std::size_t>(v)].push_back({v + n, 0});
        graph[static_cast<std::size_t>(v + n)].push_back({v, 0});
    }
    auto result = zero_one_bfs(graph, n);
    int answer = -1;
    for(int target: {n - 1, 2 * n - 1}){
        int distance = result.dist[static_cast<std::size_t>(target)];
        if(distance != -1 && (answer == -1 || distance < answer)) answer = distance;
    }
    std::cout << answer << '\n';
}
