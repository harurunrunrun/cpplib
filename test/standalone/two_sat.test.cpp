// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "../../src/algorithm/graph/constraints/two_sat.hpp"

bool brute(int n, const std::vector<std::tuple<int, int, int, int>>& clauses){
    for(int mask = 0; mask < (1 << n); mask++){
        bool ok = true;
        for(auto [i, f, j, g]: clauses){
            bool a = ((mask >> i) & 1) == f;
            bool b = ((mask >> j) & 1) == g;
            if(!a && !b) ok = false;
        }
        if(ok) return true;
    }
    return false;
}

bool check_answer(int n, const std::vector<std::tuple<int, int, int, int>>& clauses, const std::vector<int>& answer){
    if(static_cast<int>(answer.size()) != n) return false;
    for(auto [i, f, j, g]: clauses){
        bool a = answer[static_cast<std::size_t>(i)] == f;
        bool b = answer[static_cast<std::size_t>(j)] == g;
        if(!a && !b) return false;
    }
    return true;
}

bool solve(int n, const std::vector<std::tuple<int, int, int, int>>& clauses){
    TwoSat ts(n);
    for(auto [i, f, j, g]: clauses) ts.add_clause(i, f, j, g);
    bool sat = ts.satisfiable();
    if(sat) assert(check_answer(n, clauses, ts.answer));
    return sat;
}

using Implication = std::tuple<int, int, int, int>;

bool implication_holds(int mask, const Implication& implication){
    const auto [from, from_value, to, to_value] = implication;
    const bool antecedent = ((mask >> from) & 1) == from_value;
    const bool consequent = ((mask >> to) & 1) == to_value;
    return !antecedent || consequent;
}

bool brute_implications(int n, const std::vector<Implication>& implications){
    for(int mask = 0; mask < (1 << n); ++mask){
        bool ok = true;
        for(const auto& implication: implications){
            if(!implication_holds(mask, implication)){
                ok = false;
                break;
            }
        }
        if(ok) return true;
    }
    return false;
}

bool solve_implications(int n, const std::vector<Implication>& implications){
    TwoSat ts(n);
    for(const auto& [from, from_value, to, to_value]: implications){
        ts.add_implication(from, from_value, to, to_value);
    }
    const bool sat = ts.satisfiable();
    if(sat){
        int mask = 0;
        for(int variable = 0; variable < n; ++variable){
            mask |= ts.answer[static_cast<std::size_t>(variable)] << variable;
        }
        for(const auto& implication: implications){
            assert(implication_holds(mask, implication));
        }
    }
    return sat;
}

template<class Function>
void expect_two_sat_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_add_implication(){
    assert(solve_implications(1, {{0, 0, 0, 1}}));
    assert(solve_implications(1, {{0, 1, 0, 0}}));
    assert(!solve_implications(1, {
        {0, 0, 0, 1},
        {0, 1, 0, 0},
    }));
    assert(!solve_implications(2, {
        {0, 0, 0, 1},
        {0, 1, 1, 1},
        {1, 1, 1, 0},
    }));

    std::mt19937 rng(0x2A5A71U);
    for(int n = 1; n <= 7; ++n){
        for(int step = 0; step < 50; ++step){
            std::vector<Implication> implications;
            const int count = static_cast<int>(rng() % 25);
            for(int i = 0; i < count; ++i){
                implications.emplace_back(
                    static_cast<int>(rng() % n),
                    static_cast<int>(rng() & 1),
                    static_cast<int>(rng() % n),
                    static_cast<int>(rng() & 1)
                );
            }
            assert(
                solve_implications(n, implications) ==
                brute_implications(n, implications)
            );
        }
    }

    TwoSat bounds(2);
    expect_two_sat_error([&]{ bounds.add_implication(-1, false, 0, true); });
    expect_two_sat_error([&]{ bounds.add_implication(2, false, 0, true); });
    expect_two_sat_error([&]{ bounds.add_implication(0, false, -1, true); });
    expect_two_sat_error([&]{ bounds.add_implication(0, false, 2, true); });
}

void self_test(){
    {
        std::vector<std::tuple<int, int, int, int>> clauses = {
            {0, 1, 1, 1},
            {0, 0, 1, 1},
            {1, 0, 0, 1},
        };
        assert(solve(2, clauses));
    }
    {
        std::vector<std::tuple<int, int, int, int>> clauses = {
            {0, 1, 0, 1},
            {0, 0, 0, 0},
        };
        assert(!solve(1, clauses));
    }
    std::mt19937 rng(20260811);
    for(int n = 0; n <= 10; n++){
        for(int step = 0; step < 100; step++){
            std::vector<std::tuple<int, int, int, int>> clauses;
            int m = n == 0 ? 0 : static_cast<int>(rng() % 30);
            for(int q = 0; q < m; q++){
                int i = static_cast<int>(rng() % n);
                int j = static_cast<int>(rng() % n);
                int f = static_cast<int>(rng() & 1);
                int g = static_cast<int>(rng() & 1);
                clauses.push_back({i, f, j, g});
            }
            assert(solve(n, clauses) == brute(n, clauses));
        }
    }
}

int main(){
    test_add_implication();
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    TwoSat ts(n);
    for(int q = 0; q < m; q++){
        int i, f, j, g;
        std::cin >> i >> f >> j >> g;
        ts.add_clause(i, f, j, g);
    }
    std::cout << static_cast<int>(ts.satisfiable()) << '\n';
}
