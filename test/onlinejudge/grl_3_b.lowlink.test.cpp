#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/GRL_3_B"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/lowlink.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [from, to]: edges) std::cin >> from >> to;
    const auto result = lowlink(vertex_count, edges);
    std::vector<std::pair<int, int>> bridges;
    bridges.reserve(result.bridges.size());
    for(int edge_id: result.bridges){
        auto [from, to] = edges[static_cast<std::size_t>(edge_id)];
        if(to < from) std::swap(from, to);
        bridges.push_back({from, to});
    }
    std::sort(bridges.begin(), bridges.end());
    for(auto [from, to]: bridges) std::cout << from << ' ' << to << '\n';
}
