// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "../../src/algorithm/graph/difference_constraints.hpp"

constexpr long long INF = (1LL << 60);

std::vector<long long> bellman_ford(
    int n,
    int reference,
    const std::vector<DifferenceConstraint<long long>>& constraints
){
    std::vector<long long> dist(static_cast<std::size_t>(n), INF);
    dist[static_cast<std::size_t>(reference)] = 0;
    for(int iter = 0; iter < n - 1; iter++){
        bool updated = false;
        for(const auto& e: constraints){
            if(dist[static_cast<std::size_t>(e.from)] == INF) continue;
            long long nd = dist[static_cast<std::size_t>(e.from)] + e.bound;
            if(nd < dist[static_cast<std::size_t>(e.to)]){
                dist[static_cast<std::size_t>(e.to)] = nd;
                updated = true;
            }
        }
        if(!updated) break;
    }
    return dist;
}

bool has_negative_cycle(int n, const std::vector<DifferenceConstraint<long long>>& constraints){
    std::vector<long long> dist(static_cast<std::size_t>(n), 0);
    for(int iter = 0; iter < n; iter++){
        bool updated = false;
        for(const auto& e: constraints){
            long long nd = dist[static_cast<std::size_t>(e.from)] + e.bound;
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
        std::vector<DifferenceConstraint<long long>> constraints = {
            {0, 1, 3},
            {1, 2, 4},
            {0, 2, 10},
        };
        auto res = solve_difference_constraints<long long>(3, 0, constraints, INF);
        assert(res.feasible);
        assert(res.bounded[2]);
        assert(has_finite_difference_upper_bound(res, 0));
        assert(has_finite_difference_upper_bound(res, 2));
        assert(res.maximum[2] == 7);
        auto maximum = maximum_difference<long long>(3, 0, 2, constraints, INF);
        assert(maximum.feasible);
        assert(maximum.bounded);
        assert(maximum.value == 7);
        auto range = difference_range<long long>(3, 0, 2, constraints, INF);
        assert(range.feasible);
        assert(!range.has_minimum);
        assert(range.has_maximum);
        assert(range.maximum == 7);
    }
    {
        std::vector<DifferenceConstraint<long long>> constraints = {
            {0, 1, 5},
            {1, 0, -10},
        };
        assert(!solve_difference_constraints<long long>(2, 0, constraints, INF).feasible);
        assert(!maximum_difference<long long>(2, 0, 1, constraints, INF).feasible);
    }
    {
        std::vector<DifferenceConstraint<long long>> constraints;
        add_difference_equal<long long>(constraints, 0, 1, 5);
        add_difference_at_least<long long>(constraints, 1, 2, 2);
        add_difference_at_most<long long>(constraints, 1, 2, 4);
        auto range = difference_range<long long>(3, 0, 2, constraints, INF);
        assert(range.feasible);
        assert(range.has_minimum);
        assert(range.has_maximum);
        assert(range.minimum == 7);
        assert(range.maximum == 9);
    }
    {
        auto res = solve_difference_constraints<long long>(2, 0, {}, INF);
        assert(res.feasible);
        assert(!res.bounded[1]);
        assert(!has_finite_difference_upper_bound(res, 1));
        auto range = difference_range<long long>(2, 0, 1, {}, INF);
        assert(range.feasible);
        assert(!range.has_minimum);
        assert(!range.has_maximum);
    }
    {
        constexpr long long large = 3000000000000000000LL;
        std::vector<DifferenceConstraint<long long>> constraints = {{0, 1, large}};
        auto res = solve_difference_constraints<long long>(2, 0, constraints);
        assert(res.feasible);
        assert(res.bounded[1]);
        assert(res.maximum[1] == large);
    }
    {
        const auto infeasible = solve_difference_constraints<long long>(
            2, 0, {{0, 1, -1}, {1, 0, -1}}, INF
        );
        assert(!has_finite_difference_upper_bound(infeasible, 0));

        bool thrown = false;
        try{
            (void)has_finite_difference_upper_bound(
                solve_difference_constraints<long long>(2, 0, {}, INF), 2
            );
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    std::mt19937 rng(20260821);
    for(int n = 1; n <= 30; n++){
        for(int step = 0; step < 100; step++){
            std::vector<DifferenceConstraint<long long>> constraints;
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u != v && rng() % 7 == 0){
                        constraints.push_back({u, v, static_cast<int>(rng() % 21) - 10});
                    }
                }
            }
            int reference = static_cast<int>(rng() % n);
            auto res = solve_difference_constraints<long long>(n, reference, constraints, INF);
            bool negative = has_negative_cycle(n, constraints);
            assert(res.feasible == !negative);
            if(res.feasible){
                auto expected = bellman_ford(n, reference, constraints);
                for(int v = 0; v < n; v++){
                    assert(res.bounded[static_cast<std::size_t>(v)] == (expected[static_cast<std::size_t>(v)] != INF));
                    assert(res.maximum[static_cast<std::size_t>(v)] == expected[static_cast<std::size_t>(v)]);
                    auto single = maximum_difference<long long>(n, reference, v, constraints, INF);
                    assert(single.feasible);
                    assert(single.bounded == res.bounded[static_cast<std::size_t>(v)]);
                    if(single.bounded) assert(single.value == expected[static_cast<std::size_t>(v)]);
                }
            }
        }
    }
}

int main(){
    int n, m, q;
    if(!(std::cin >> n >> m >> q)){
        self_test();
        return 0;
    }
    std::vector<DifferenceConstraint<long long>> constraints;
    constraints.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int from, to;
        long long bound;
        std::cin >> from >> to >> bound;
        constraints.push_back({from, to, bound});
    }
    while(q--){
        std::string type;
        int reference, variable;
        std::cin >> type >> reference >> variable;
        if(type == "MAX"){
            auto res = maximum_difference<long long>(n, reference, variable, constraints, INF);
            if(!res.feasible) std::cout << "infeasible\n";
            else if(!res.bounded) std::cout << "inf\n";
            else std::cout << res.value << '\n';
        }else if(type == "RANGE"){
            auto res = difference_range<long long>(n, reference, variable, constraints, INF);
            if(!res.feasible){
                std::cout << "infeasible\n";
            }else{
                if(res.has_minimum) std::cout << res.minimum;
                else std::cout << "-inf";
                std::cout << ' ';
                if(res.has_maximum) std::cout << res.maximum;
                else std::cout << "inf";
                std::cout << '\n';
            }
        }
    }
}
