// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc277/tasks/abc277_e

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/shortest_path_with_mode_switches.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count, switch_count;
    std::cin >> vertex_count >> edge_count >> switch_count;
    std::vector<ModeSwitchEdge> edges(static_cast<std::size_t>(edge_count));
    for(auto& edge: edges){
        std::cin >> edge.from >> edge.to >> edge.mode;
        --edge.from;
        --edge.to;
    }
    std::vector<int> switch_vertices(static_cast<std::size_t>(switch_count));
    for(int& vertex: switch_vertices){
        std::cin >> vertex;
        --vertex;
    }
    std::cout << shortest_path_with_mode_switches(
        vertex_count,
        edges,
        switch_vertices
    ) << '\n';
}
