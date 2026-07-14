// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "../../src/algorithm/matching/minimum_weight_unicyclic_matching.hpp"

using Edge = MinimumWeightUnicyclicMatchingEdge<long long>;

std::pair<int, long long> brute(int n, const std::vector<Edge>& edges){
    std::vector<char> used(static_cast<std::size_t>(n), 0);
    std::pair<int, long long> best = {-1, 0};
    auto dfs = [&](auto&& self, int id, int count, long long cost) -> void {
        if(id == static_cast<int>(edges.size())){
            if(count > best.first || (count == best.first && cost < best.second)){
                best = {count, cost};
            }
            return;
        }
        self(self, id + 1, count, cost);
        const auto& edge = edges[static_cast<std::size_t>(id)];
        if(!used[static_cast<std::size_t>(edge.from)] && !used[static_cast<std::size_t>(edge.to)]){
            used[static_cast<std::size_t>(edge.from)] = used[static_cast<std::size_t>(edge.to)] = 1;
            self(self, id + 1, count + 1, cost + edge.cost);
            used[static_cast<std::size_t>(edge.from)] = used[static_cast<std::size_t>(edge.to)] = 0;
        }
    };
    dfs(dfs, 0, 0, 0);
    return best;
}

bool valid(int n, const std::vector<Edge>& edges, const MinimumWeightUnicyclicMatchingResult<long long>& result){
    std::map<std::pair<int, int>, long long> minimum_cost;
    for(const auto& edge: edges){
        int u = std::min(edge.from, edge.to);
        int v = std::max(edge.from, edge.to);
        auto key = std::pair<int, int>{u, v};
        if(!minimum_cost.count(key)) minimum_cost[key] = edge.cost;
        minimum_cost[key] = std::min(minimum_cost[key], edge.cost);
    }
    int count = 0;
    long long cost = 0;
    for(int i = 0; i < n; i++){
        int j = result.match[static_cast<std::size_t>(i)];
        if(j == -1) continue;
        if(j < 0 || n <= j) return false;
        if(result.match[static_cast<std::size_t>(j)] != i) return false;
        if(i < j){
            count++;
            auto key = std::pair<int, int>{i, j};
            if(!minimum_cost.count(key)) return false;
            cost += minimum_cost[key];
        }
    }
    return count == result.size && cost == result.cost;
}

std::vector<Edge> make_case(int n, std::mt19937& rng){
    std::vector<Edge> edges;
    if(n <= 1) return edges;
    int cycle_size = n == 2 ? 2 : 3 + static_cast<int>(rng() % static_cast<unsigned>(n - 2));
    auto weight = [&]() -> long long {
        return static_cast<int>(rng() % 21) - 10;
    };
    if(cycle_size == 2){
        edges.push_back({0, 1, weight()});
        edges.push_back({0, 1, weight()});
    }else{
        for(int i = 0; i < cycle_size; i++){
            edges.push_back({i, (i + 1) % cycle_size, weight()});
        }
    }
    for(int v = cycle_size; v < n; v++){
        int p = static_cast<int>(rng() % static_cast<unsigned>(v));
        edges.push_back({p, v, weight()});
    }
    std::shuffle(edges.begin(), edges.end(), rng);
    return edges;
}

void self_test(){
    {
        bool thrown = false;
        try{
            (void)minimum_weight_unicyclic_matching<long long>(-1, {});
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        std::vector<Edge> edges = {
            {0, 1, 5},
            {1, 2, 1},
            {2, 0, 1},
            {3, 4, -2},
        };
        auto res = minimum_weight_unicyclic_matching<long long>(5, edges);
        assert(valid(5, edges, res));
        assert(res.size == 2);
        assert(res.cost == -1);
    }
    std::mt19937 rng(20260905);
    for(int n = 0; n <= 11; n++){
        for(int step = 0; step < 80; step++){
            auto edges = make_case(n, rng);
            auto res = minimum_weight_unicyclic_matching<long long>(n, edges);
            auto expected = brute(n, edges);
            assert(valid(n, edges, res));
            assert(res.size == expected.first);
            assert(res.cost == expected.second);

            if(!edges.empty()){
                edges.pop_back();
                auto forest_result = minimum_weight_unicyclic_matching<long long>(n, edges);
                auto forest_expected = brute(n, edges);
                assert(valid(n, edges, forest_result));
                assert(forest_result.size == forest_expected.first);
                assert(forest_result.cost == forest_expected.second);
            }
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<Edge> edges(static_cast<std::size_t>(m));
    for(auto& edge: edges){
        std::cin >> edge.from >> edge.to >> edge.cost;
    }
    auto res = minimum_weight_unicyclic_matching<long long>(n, edges);
    std::cout << res.size << ' ' << res.cost << '\n';
}
