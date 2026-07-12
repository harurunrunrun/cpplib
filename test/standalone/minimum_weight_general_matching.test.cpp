// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/matching/minimum_weight_general_matching.hpp"

std::pair<int, long long> brute(
    int n,
    const std::vector<MinimumWeightGeneralMatchingEdge<long long>>& edges
){
    std::vector<std::vector<char>> exists(static_cast<std::size_t>(n), std::vector<char>(static_cast<std::size_t>(n), 0));
    std::vector<std::vector<long long>> cost(static_cast<std::size_t>(n), std::vector<long long>(static_cast<std::size_t>(n), 1LL << 60));
    for(auto e: edges){
        if(e.from == e.to) continue;
        exists[static_cast<std::size_t>(e.from)][static_cast<std::size_t>(e.to)] = 1;
        exists[static_cast<std::size_t>(e.to)][static_cast<std::size_t>(e.from)] = 1;
        cost[static_cast<std::size_t>(e.from)][static_cast<std::size_t>(e.to)] =
            std::min(cost[static_cast<std::size_t>(e.from)][static_cast<std::size_t>(e.to)], e.cost);
        cost[static_cast<std::size_t>(e.to)][static_cast<std::size_t>(e.from)] =
            std::min(cost[static_cast<std::size_t>(e.to)][static_cast<std::size_t>(e.from)], e.cost);
    }
    std::vector<char> used(static_cast<std::size_t>(n), 0);
    int best_size = 0;
    long long best_cost = 0;
    auto dfs = [&](auto&& self, int size, long long total) -> void {
        int v = -1;
        for(int i = 0; i < n; i++){
            if(!used[static_cast<std::size_t>(i)]){
                v = i;
                break;
            }
        }
        if(v == -1){
            if(best_size < size || (best_size == size && total < best_cost)){
                best_size = size;
                best_cost = total;
            }
            return;
        }
        used[static_cast<std::size_t>(v)] = 1;
        self(self, size, total);
        for(int u = v + 1; u < n; u++){
            if(used[static_cast<std::size_t>(u)] || !exists[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)]) continue;
            used[static_cast<std::size_t>(u)] = 1;
            self(self, size + 1, total + cost[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)]);
            used[static_cast<std::size_t>(u)] = 0;
        }
        used[static_cast<std::size_t>(v)] = 0;
    };
    dfs(dfs, 0, 0);
    return {best_size, best_cost};
}

bool valid(int n, const MinimumWeightGeneralMatchingResult<long long>& result){
    int count = 0;
    for(int i = 0; i < n; i++){
        int j = result.match[static_cast<std::size_t>(i)];
        if(j == -1) continue;
        if(j < 0 || n <= j) return false;
        if(result.match[static_cast<std::size_t>(j)] != i) return false;
        if(i < j) count++;
    }
    return count == result.size;
}

void self_test(){
    {
        std::vector<MinimumWeightGeneralMatchingEdge<long long>> edges = {
            {0, 1, 5},
            {1, 2, 1},
            {2, 0, 1},
            {3, 4, -2},
        };
        auto res = minimum_weight_general_matching<long long>(5, edges);
        assert(res.size == 2);
        assert(res.cost == -1);
        assert(valid(5, res));
    }
    {
        std::vector<MinimumWeightGeneralMatchingEdge<long long>> edges = {
            {0, 1, 10},
            {1, 0, -5},
            {2, 3, 7},
            {1, 1, -100},
        };
        auto res = minimum_weight_general_matching<long long>(4, edges);
        assert(res.size == 2);
        assert(res.cost == 2);
        assert(valid(4, res));
    }
    std::mt19937 rng(20260829);
    for(int n = 0; n <= 12; n++){
        for(int step = 0; step < 60; step++){
            std::vector<MinimumWeightGeneralMatchingEdge<long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 4 == 0){
                        edges.push_back({u, v, static_cast<int>(rng() % 21) - 10});
                    }
                }
            }
            auto expected = brute(n, edges);
            auto actual = minimum_weight_general_matching<long long>(n, edges);
            assert(valid(n, actual));
            assert(actual.size == expected.first);
            assert(actual.cost == expected.second);
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<MinimumWeightGeneralMatchingEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int u, v;
        long long c;
        std::cin >> u >> v >> c;
        edges.push_back({u, v, c});
    }
    auto res = minimum_weight_general_matching<long long>(n, edges);
    assert(valid(n, res));
    std::cout << res.size << ' ' << res.cost << '\n';
}
