// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/incremental_minimum_spanning_forest

#include <iostream>
#include <memory>

#include "../../src/structure/graph/spanning_forest/incremental_minimum_spanning_forest.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    auto forest = std::make_unique<
        IncrementalMinimumSpanningForest<long long, 500000, 1000000>
    >(vertex_count);
    for(int edge = 0; edge < edge_count; ++edge){
        int first, second;
        long long weight;
        std::cin >> first >> second >> weight;
        if(edge) std::cout << ' ';
        std::cout << forest->add_edge(first, second, weight);
    }
    std::cout << '\n';
}
