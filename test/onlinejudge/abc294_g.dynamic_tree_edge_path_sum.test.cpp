// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc294/tasks/abc294_g

#include <iostream>
#include <tuple>
#include <vector>

#include "../../src/structure/tree/dynamic_tree_edge_path_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<std::tuple<int, int, long long>> edges;
    edges.reserve(static_cast<std::size_t>(n - 1));
    for(int i = 1; i < n; ++i){
        int u, v;
        long long weight;
        std::cin >> u >> v >> weight;
        --u;
        --v;
        edges.push_back({u, v, weight});
    }
    DynamicTreeEdgePathSum<long long> tree(n, edges);

    int q;
    std::cin >> q;
    while(q--){
        int type, x;
        long long y;
        std::cin >> type >> x >> y;
        if(type == 1){
            tree.set_edge(x - 1, y);
        }else{
            std::cout << tree.path_sum(x - 1, static_cast<int>(y) - 1) << '\n';
        }
    }
}
