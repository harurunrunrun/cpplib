// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/constraints/chromatic_number.hpp"

bool can_color(
    const std::vector<std::vector<int>>& graph,
    int colors,
    std::vector<int>& color,
    int v
){
    const int n = static_cast<int>(graph.size());
    if(v == n) return true;
    for(int c = 0; c < colors; c++){
        bool ok = true;
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(to < v && color[static_cast<std::size_t>(to)] == c) ok = false;
        }
        if(ok){
            color[static_cast<std::size_t>(v)] = c;
            if(can_color(graph, colors, color, v + 1)) return true;
            color[static_cast<std::size_t>(v)] = -1;
        }
    }
    return false;
}

int brute(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    for(int colors = 0; colors <= n; colors++){
        std::vector<int> color(static_cast<std::size_t>(n), -1);
        if(can_color(graph, colors, color, 0)) return colors;
    }
    return n;
}

void self_test(){
    {
        std::vector<std::vector<int>> graph(5);
        auto add = [&](int u, int v){
            graph[static_cast<std::size_t>(u)].push_back(v);
            graph[static_cast<std::size_t>(v)].push_back(u);
        };
        add(0, 1);
        add(1, 2);
        add(2, 3);
        add(3, 4);
        add(4, 0);
        assert(chromatic_number(graph) == 3);
    }
    {
        std::vector<std::vector<int>> graph(4);
        for(int i = 0; i < 4; i++){
            for(int j = i + 1; j < 4; j++){
                graph[static_cast<std::size_t>(i)].push_back(j);
                graph[static_cast<std::size_t>(j)].push_back(i);
            }
        }
        assert(chromatic_number(graph) == 4);
    }
    std::mt19937 rng(20260813);
    for(int n = 0; n <= 12; n++){
        for(int step = 0; step < 60; step++){
            std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 3 == 0){
                        graph[static_cast<std::size_t>(u)].push_back(v);
                        graph[static_cast<std::size_t>(v)].push_back(u);
                    }
                }
            }
            assert(chromatic_number(graph) == brute(graph));
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
    std::cout << chromatic_number(graph) << '\n';
}
