// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/two_edge_connected_components.hpp"

int components_without_bridges(int n, const std::vector<std::pair<int, int>>& edges){
    auto low = lowlink(n, edges);
    std::vector<char> bridge(edges.size(), 0);
    for(int id: low.bridges) bridge[static_cast<std::size_t>(id)] = 1;
    std::vector<std::vector<std::pair<int, int>>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < static_cast<int>(edges.size()); i++){
        auto [u, v] = edges[static_cast<std::size_t>(i)];
        graph[static_cast<std::size_t>(u)].push_back({v, i});
        graph[static_cast<std::size_t>(v)].push_back({u, i});
    }
    std::vector<char> seen(static_cast<std::size_t>(n), 0);
    int count = 0;
    for(int s = 0; s < n; s++){
        if(seen[static_cast<std::size_t>(s)]) continue;
        count++;
        std::vector<int> stack{s};
        seen[static_cast<std::size_t>(s)] = 1;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            for(auto [to, id]: graph[static_cast<std::size_t>(v)]){
                if(bridge[static_cast<std::size_t>(id)]) continue;
                if(!seen[static_cast<std::size_t>(to)]){
                    seen[static_cast<std::size_t>(to)] = 1;
                    stack.push_back(to);
                }
            }
        }
    }
    return count;
}

void self_test(){
    {
        std::vector<std::pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 0}, {1, 3}};
        auto res = two_edge_connected_components(4, edges);
        assert(res.count == 2);
        assert(res.id[0] == res.id[1] && res.id[1] == res.id[2]);
        assert(res.id[1] != res.id[3]);
    }
    std::mt19937 rng(20260818);
    for(int n = 0; n <= 30; n++){
        for(int step = 0; step < 80; step++){
            std::vector<std::pair<int, int>> edges;
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 4 == 0) edges.push_back({u, v});
                    if(rng() % 19 == 0) edges.push_back({u, v});
                }
            }
            auto res = two_edge_connected_components(n, edges);
            assert(res.count == components_without_bridges(n, edges));
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        edges.push_back({u, v});
    }
    auto res = two_edge_connected_components(n, edges);
    std::cout << res.count << '\n';
}
