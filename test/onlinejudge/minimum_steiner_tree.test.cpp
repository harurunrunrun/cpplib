// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/minimum_steiner_tree

#include <iostream>
#include <limits>
#include <vector>

#include "../../src/algorithm/graph/specialized/minimum_steiner_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<std::vector<MinimumSteinerTreeEdge<long long>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(int id = 0; id < edge_count; ++id){
        int left, right;
        long long cost;
        std::cin >> left >> right >> cost;
        graph[static_cast<std::size_t>(left)].push_back({right, cost, id});
        graph[static_cast<std::size_t>(right)].push_back({left, cost, id});
    }
    int terminal_count;
    std::cin >> terminal_count;
    std::vector<int> terminals(static_cast<std::size_t>(terminal_count));
    for(int& terminal: terminals) std::cin >> terminal;

    const auto result = minimum_steiner_tree(
        graph,
        terminals,
        std::numeric_limits<long long>::max() / 4
    );
    std::cout << result.cost << ' ' << result.edge_ids.size() << '\n';
    for(std::size_t index = 0; index < result.edge_ids.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << result.edge_ids[index];
    }
    std::cout << '\n';
}
