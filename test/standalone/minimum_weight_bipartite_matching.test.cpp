// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/matching/minimum_weight_bipartite_matching.hpp"

std::pair<int, long long> brute(
    int left_size,
    int right_size,
    const std::vector<MinimumWeightBipartiteMatchingEdge<long long>>& edges
){
    std::vector<std::vector<std::pair<int, long long>>> graph(static_cast<std::size_t>(left_size));
    for(const auto& e: edges) graph[static_cast<std::size_t>(e.left)].push_back({e.right, e.cost});
    int best_size = 0;
    long long best_cost = 0;
    std::vector<char> used(static_cast<std::size_t>(right_size), 0);
    auto dfs = [&](auto&& self, int v, int size, long long cost) -> void {
        if(v == left_size){
            if(best_size < size || (best_size == size && cost < best_cost)){
                best_size = size;
                best_cost = cost;
            }
            return;
        }
        self(self, v + 1, size, cost);
        for(auto [to, c]: graph[static_cast<std::size_t>(v)]){
            if(!used[static_cast<std::size_t>(to)]){
                used[static_cast<std::size_t>(to)] = 1;
                self(self, v + 1, size + 1, cost + c);
                used[static_cast<std::size_t>(to)] = 0;
            }
        }
    };
    dfs(dfs, 0, 0, 0);
    return {best_size, best_cost};
}

void self_test(){
    {
        std::vector<MinimumWeightBipartiteMatchingEdge<long long>> edges = {
            {0, 0, 5},
            {0, 1, 1},
            {1, 0, 2},
            {1, 1, 10},
        };
        auto res = minimum_weight_bipartite_matching<long long>(2, 2, edges);
        assert(res.size == 2);
        assert(res.cost == 3);
    }
    std::mt19937 rng(20260824);
    for(int l = 0; l <= 8; l++){
        for(int r = 0; r <= 8; r++){
            for(int step = 0; step < 80; step++){
                std::vector<MinimumWeightBipartiteMatchingEdge<long long>> edges;
                for(int i = 0; i < l; i++){
                    for(int j = 0; j < r; j++){
                        if(rng() % 3 == 0){
                            edges.push_back({i, j, static_cast<int>(rng() % 21) - 10});
                        }
                    }
                }
                auto expected = brute(l, r, edges);
                auto actual = minimum_weight_bipartite_matching<long long>(l, r, edges);
                assert(actual.size == expected.first);
                assert(actual.cost == expected.second);
            }
        }
    }
}

int main(){
    int l, r, m;
    if(!(std::cin >> l >> r >> m)){
        self_test();
        return 0;
    }
    std::vector<MinimumWeightBipartiteMatchingEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int u, v;
        long long c;
        std::cin >> u >> v >> c;
        edges.push_back({u, v, c});
    }
    auto res = minimum_weight_bipartite_matching<long long>(l, r, edges);
    std::cout << res.size << ' ' << res.cost << '\n';
}
