// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DPL_2_B

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/specialized/chinese_postman.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<ChinesePostmanEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    for(int index = 0; index < edge_count; ++index){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        edges.push_back({from, to, cost});
    }
    const auto answer = undirected_chinese_postman<long long>(vertex_count, edges);
    std::cout << answer->cost << '\n';
}
