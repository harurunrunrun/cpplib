#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/GRL_2_A"

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/kruskal_mst.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<KruskalEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    while(edge_count--){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        edges.push_back({from, to, cost});
    }
    std::cout << kruskal_mst(vertex_count, edges).cost << '\n';
}
