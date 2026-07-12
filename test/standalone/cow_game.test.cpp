// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/cow_game.hpp"

constexpr long long INF = (1LL << 60);

std::vector<long long> bellman_ford(
    int n,
    int source,
    const std::vector<CowGameConstraint<long long>>& constraints
){
    std::vector<long long> dist(static_cast<std::size_t>(n), INF);
    dist[static_cast<std::size_t>(source)] = 0;
    for(int iter = 0; iter < n - 1; iter++){
        bool updated = false;
        for(const auto& e: constraints){
            if(dist[static_cast<std::size_t>(e.from)] == INF) continue;
            long long nd = dist[static_cast<std::size_t>(e.from)] + e.cost;
            if(nd < dist[static_cast<std::size_t>(e.to)]){
                dist[static_cast<std::size_t>(e.to)] = nd;
                updated = true;
            }
        }
        if(!updated) break;
    }
    return dist;
}

bool has_negative_cycle(int n, const std::vector<CowGameConstraint<long long>>& constraints){
    std::vector<long long> dist(static_cast<std::size_t>(n), 0);
    for(int iter = 0; iter < n; iter++){
        bool updated = false;
        for(const auto& e: constraints){
            long long nd = dist[static_cast<std::size_t>(e.from)] + e.cost;
            if(nd < dist[static_cast<std::size_t>(e.to)]){
                dist[static_cast<std::size_t>(e.to)] = nd;
                updated = true;
            }
        }
        if(!updated) return false;
        if(iter == n - 1) return true;
    }
    return false;
}

void self_test(){
    {
        std::vector<CowGameConstraint<long long>> constraints = {
            {0, 1, 3},
            {1, 2, 4},
            {0, 2, 10},
        };
        auto res = cow_game<long long>(3, 0, constraints, INF);
        assert(res.feasible);
        assert(res.bounded[2]);
        assert(res.maximum[2] == 7);
    }
    {
        std::vector<CowGameConstraint<long long>> constraints = {
            {0, 1, 5},
            {1, 0, -10},
        };
        assert(!cow_game<long long>(2, 0, constraints, INF).feasible);
    }
    {
        auto res = cow_game<long long>(2, 0, {}, INF);
        assert(res.feasible);
        assert(!res.bounded[1]);
    }
    std::mt19937 rng(20260821);
    for(int n = 1; n <= 30; n++){
        for(int step = 0; step < 100; step++){
            std::vector<CowGameConstraint<long long>> constraints;
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u != v && rng() % 7 == 0){
                        constraints.push_back({u, v, static_cast<int>(rng() % 21) - 10});
                    }
                }
            }
            int source = static_cast<int>(rng() % n);
            auto res = cow_game<long long>(n, source, constraints, INF);
            bool negative = has_negative_cycle(n, constraints);
            assert(res.feasible == !negative);
            if(res.feasible){
                auto expected = bellman_ford(n, source, constraints);
                for(int v = 0; v < n; v++){
                    assert(res.bounded[static_cast<std::size_t>(v)] == (expected[static_cast<std::size_t>(v)] != INF));
                    assert(res.maximum[static_cast<std::size_t>(v)] == expected[static_cast<std::size_t>(v)]);
                }
            }
        }
    }
}

int main(){
    int n, m, source, target;
    if(!(std::cin >> n >> m >> source >> target)){
        self_test();
        return 0;
    }
    std::vector<CowGameConstraint<long long>> constraints;
    constraints.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        constraints.push_back({from, to, cost});
    }
    auto res = cow_game<long long>(n, source, constraints, INF);
    if(!res.feasible) std::cout << "infeasible\n";
    else if(!res.bounded[static_cast<std::size_t>(target)]) std::cout << "unbounded\n";
    else std::cout << res.maximum[static_cast<std::size_t>(target)] << '\n';
}
