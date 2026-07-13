// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc294/tasks/abc294_g

#include <iostream>
#include <tuple>
#include <vector>

#include "../../src/algorithm/tree/heavy_light_decomposition.hpp"
#include "../../src/structure/other/fenwick_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    HeavyLightDecomposition hld(n);
    std::vector<std::tuple<int, int, long long>> edges;
    edges.reserve(static_cast<std::size_t>(n - 1));
    for(int i = 1; i < n; ++i){
        int u, v;
        long long weight;
        std::cin >> u >> v >> weight;
        --u;
        --v;
        hld.add_edge(u, v);
        edges.push_back({u, v, weight});
    }
    hld.build();
    FenwickTree<long long, 200000> fenwick(n);
    std::vector<int> edge_position(static_cast<std::size_t>(n - 1));
    for(int i = 0; i + 1 < n; ++i){
        auto [u, v, weight] = edges[static_cast<std::size_t>(i)];
        const int child = hld.parent(u) == v ? u : v;
        edge_position[static_cast<std::size_t>(i)] = hld.in(child);
        fenwick.add(hld.in(child), weight);
    }

    int q;
    std::cin >> q;
    while(q--){
        int type, x;
        long long y;
        std::cin >> type >> x >> y;
        if(type == 1){
            fenwick.set(edge_position[static_cast<std::size_t>(x - 1)], y);
        }else{
            long long answer = 0;
            for(const auto& [left, right]: hld.path_query(x - 1, static_cast<int>(y) - 1, false)){
                answer += fenwick.sum(left, right);
            }
            std::cout << answer << '\n';
        }
    }
}
