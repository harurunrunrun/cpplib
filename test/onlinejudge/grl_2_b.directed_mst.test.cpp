#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/GRL_2_B"

#include <iostream>
#include <limits>
#include <vector>

#include "../../src/algorithm/graph/directed_mst.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count, edge_count, root;
    std::cin >> vertex_count >> edge_count >> root;
    std::vector<DirectedMstEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    while(edge_count--){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        edges.push_back({from, to, cost});
    }
    const auto result = directed_mst(
        vertex_count, root, std::move(edges),
        std::numeric_limits<long long>::max() / 4
    );
    std::cout << result.cost << '\n';
}
