// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "../../src/algorithm/matching/flow/burn_bury.hpp"

struct PairCost{
    int i;
    int j;
    long long c00;
    long long c01;
    long long c10;
    long long c11;
};

long long brute(
    int n,
    const std::vector<std::pair<long long, long long>>& one,
    const std::vector<PairCost>& pair
){
    long long best = (1LL << 60);
    for(int mask = 0; mask < (1 << n); mask++){
        long long cost = 0;
        for(int i = 0; i < n; i++){
            cost += (mask >> i & 1) ? one[static_cast<std::size_t>(i)].second : one[static_cast<std::size_t>(i)].first;
        }
        for(const auto& p: pair){
            int a = mask >> p.i & 1;
            int b = mask >> p.j & 1;
            if(a == 0 && b == 0) cost += p.c00;
            if(a == 0 && b == 1) cost += p.c01;
            if(a == 1 && b == 0) cost += p.c10;
            if(a == 1 && b == 1) cost += p.c11;
        }
        best = std::min(best, cost);
    }
    return best;
}

long long assignment_cost(
    int n,
    const std::vector<std::pair<long long, long long>>& one,
    const std::vector<PairCost>& pair,
    const std::vector<int>& assignment
){
    assert(assignment.size() == static_cast<std::size_t>(n));
    long long cost = 0;
    for(int i = 0; i < n; ++i){
        assert(assignment[static_cast<std::size_t>(i)] == 0
            || assignment[static_cast<std::size_t>(i)] == 1);
        cost += assignment[static_cast<std::size_t>(i)]
            ? one[static_cast<std::size_t>(i)].second
            : one[static_cast<std::size_t>(i)].first;
    }
    for(const auto& p: pair){
        const int left = assignment[static_cast<std::size_t>(p.i)];
        const int right = assignment[static_cast<std::size_t>(p.j)];
        if(left == 0 && right == 0) cost += p.c00;
        if(left == 0 && right == 1) cost += p.c01;
        if(left == 1 && right == 0) cost += p.c10;
        if(left == 1 && right == 1) cost += p.c11;
    }
    return cost;
}

void self_test(){
    {
        BurnBury<long long> graph(2);
        graph.add_one_cost(0, 3, 0);
        graph.add_one_cost(1, 0, 3);
        graph.add_pair_cost(0, 1, 0, 5, 5, 0);
        auto res = graph.solve();
        assert(res.cost == 3);
        assert(assignment_cost(
            2,
            {{3, 0}, {0, 3}},
            {{0, 1, 0, 5, 5, 0}},
            res.assignment
        ) == res.cost);
    }
    {
        BurnBury<long long> graph(2);
        graph.add_one_delta(0, 5);
        graph.add_one_delta(1, -4);
        const auto result = graph.solve();
        assert(result.cost == -4);
        assert((result.assignment == std::vector<int>{0, 1}));
    }
    {
        BurnBury<long long> graph(2);
        bool thrown = false;
        try{
            graph.add_pair_cost(0, 1, 0, 0, 0, 1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    std::mt19937 rng(20260825);
    for(int n = 0; n <= 12; n++){
        for(int step = 0; step < 80; step++){
            BurnBury<long long> graph(n);
            std::vector<std::pair<long long, long long>> one(static_cast<std::size_t>(n));
            std::vector<PairCost> pair;
            for(int i = 0; i < n; i++){
                long long c0 = static_cast<int>(rng() % 21) - 10;
                long long c1 = static_cast<int>(rng() % 21) - 10;
                one[static_cast<std::size_t>(i)] = {c0, c1};
                graph.add_one_cost(i, c0, c1);
            }
            for(int i = 0; i < n; i++){
                for(int j = i + 1; j < n; j++){
                    if(rng() % 7 != 0) continue;
                    long long c00 = static_cast<int>(rng() % 11) - 5;
                    long long c01 = static_cast<int>(rng() % 11) - 5;
                    long long c10 = static_cast<int>(rng() % 11) - 5;
                    long long c11 = static_cast<int>(rng() % 11) - 5;
                    long long need = c00 + c11 - c01 - c10;
                    if(need > 0) c01 += need;
                    pair.push_back({i, j, c00, c01, c10, c11});
                    graph.add_pair_cost(i, j, c00, c01, c10, c11);
                }
            }
            const auto result = graph.solve();
            assert(result.cost == brute(n, one, pair));
            assert(assignment_cost(n, one, pair, result.assignment) == result.cost);
        }
    }
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    BurnBury<long long> graph(n);
    for(int i = 0; i < n; i++){
        long long c0, c1;
        std::cin >> c0 >> c1;
        graph.add_one_cost(i, c0, c1);
    }
    for(int k = 0; k < q; k++){
        int i, j;
        long long c00, c01, c10, c11;
        std::cin >> i >> j >> c00 >> c01 >> c10 >> c11;
        graph.add_pair_cost(i, j, c00, c01, c10, c11);
    }
    std::cout << graph.solve().cost << '\n';
}
