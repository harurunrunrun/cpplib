// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/cycle_detection

#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/traversal/cycle_detection.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [from, to]: edges) std::cin >> from >> to;
    const auto cycle = directed_cycle_with_edges(n, edges);
    if(cycle.edges.empty()){
        std::cout << -1 << '\n';
        return 0;
    }
    std::cout << cycle.edges.size() << '\n';
    for(int id: cycle.edges) std::cout << id << '\n';
}
