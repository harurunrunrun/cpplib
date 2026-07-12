// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/strongly_connected_components.hpp"

std::vector<std::vector<char>> reachability(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<char>> reach(static_cast<std::size_t>(n), std::vector<char>(static_cast<std::size_t>(n), 0));
    for(int s = 0; s < n; s++){
        std::vector<int> stack{s};
        reach[static_cast<std::size_t>(s)][static_cast<std::size_t>(s)] = 1;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            for(int to: graph[static_cast<std::size_t>(v)]){
                if(!reach[static_cast<std::size_t>(s)][static_cast<std::size_t>(to)]){
                    reach[static_cast<std::size_t>(s)][static_cast<std::size_t>(to)] = 1;
                    stack.push_back(to);
                }
            }
        }
    }
    return reach;
}

int brute_count(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    auto reach = reachability(graph);
    std::vector<char> used(static_cast<std::size_t>(n), 0);
    int count = 0;
    for(int v = 0; v < n; v++){
        if(used[static_cast<std::size_t>(v)]) continue;
        count++;
        for(int u = 0; u < n; u++){
            if(reach[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)] &&
               reach[static_cast<std::size_t>(u)][static_cast<std::size_t>(v)]){
                used[static_cast<std::size_t>(u)] = 1;
            }
        }
    }
    return count;
}

void self_test(){
    {
        std::vector<std::vector<int>> graph(5);
        graph[0] = {1};
        graph[1] = {2};
        graph[2] = {0, 3};
        graph[3] = {4};
        graph[4] = {3};
        auto res = strongly_connected_components(graph);
        assert(res.count == 2);
        assert(res.id[0] == res.id[1] && res.id[1] == res.id[2]);
        assert(res.id[3] == res.id[4]);
        assert(res.id[0] != res.id[3]);
    }
    std::mt19937 rng(20260810);
    for(int n = 0; n <= 25; n++){
        for(int step = 0; step < 60; step++){
            std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u != v && rng() % 8 == 0) graph[static_cast<std::size_t>(u)].push_back(v);
                }
            }
            auto res = strongly_connected_components(graph);
            assert(res.count == brute_count(graph));
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
    }
    auto res = strongly_connected_components(graph);
    std::cout << res.count << '\n';
}
