// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include "../../src/algorithm/graph/two_sat.hpp"

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
