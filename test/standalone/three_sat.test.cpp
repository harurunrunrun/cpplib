// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include "../../src/algorithm/graph/three_sat.hpp"

using ClauseTuple = std::array<std::pair<int, int>, 3>;

ThreeSatClause make_clause(const ClauseTuple& tuple){
    ThreeSatClause clause;
    for(int i = 0; i < 3; i++){
        clause.literal[static_cast<std::size_t>(i)] = {tuple[static_cast<std::size_t>(i)].first, static_cast<bool>(tuple[static_cast<std::size_t>(i)].second)};
    }
    return clause;
}

bool brute(int n, const std::vector<ThreeSatClause>& clauses){
    for(int mask = 0; mask < (1 << n); mask++){
        bool ok = true;
        for(const auto& clause: clauses){
            bool satisfied = false;
            for(auto lit: clause.literal){
                if(((mask >> lit.variable) & 1) == static_cast<int>(lit.value)){
                    satisfied = true;
                }
            }
            if(!satisfied) ok = false;
        }
        if(ok) return true;
    }
    return false;
}

bool check_answer(const std::vector<ThreeSatClause>& clauses, const std::vector<int>& assignment){
    for(const auto& clause: clauses){
        bool satisfied = false;
        for(auto lit: clause.literal){
            if(assignment[static_cast<std::size_t>(lit.variable)] == static_cast<int>(lit.value)){
                satisfied = true;
            }
        }
        if(!satisfied) return false;
    }
    return true;
}

void self_test(){
    {
        std::vector<ThreeSatClause> clauses = {
            make_clause({{{0, 1}, {1, 1}, {2, 0}}}),
            make_clause({{{0, 0}, {1, 1}, {2, 1}}}),
        };
        auto res = three_sat(3, clauses);
        assert(res.satisfiable);
        assert(check_answer(clauses, res.assignment));
    }
    {
        std::vector<ThreeSatClause> clauses = {
            make_clause({{{0, 1}, {0, 1}, {0, 1}}}),
            make_clause({{{0, 0}, {0, 0}, {0, 0}}}),
        };
        assert(!three_sat(1, clauses).satisfiable);
    }
    std::mt19937 rng(20260812);
    for(int n = 0; n <= 12; n++){
        for(int step = 0; step < 80; step++){
            std::vector<ThreeSatClause> clauses;
            int m = n == 0 ? 0 : static_cast<int>(rng() % 40);
            for(int i = 0; i < m; i++){
                ThreeSatClause clause;
                for(int j = 0; j < 3; j++){
                    clause.literal[static_cast<std::size_t>(j)] = {
                        static_cast<int>(rng() % n),
                        static_cast<bool>(rng() & 1)
                    };
                }
                clauses.push_back(clause);
            }
            auto res = three_sat(n, clauses);
            assert(res.satisfiable == brute(n, clauses));
            if(res.satisfiable) assert(check_answer(clauses, res.assignment));
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<ThreeSatClause> clauses(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        for(int j = 0; j < 3; j++){
            int v, value;
            std::cin >> v >> value;
            clauses[static_cast<std::size_t>(i)].literal[static_cast<std::size_t>(j)] = {v, static_cast<bool>(value)};
        }
    }
    auto res = three_sat(n, clauses);
    std::cout << static_cast<int>(res.satisfiable) << '\n';
}
