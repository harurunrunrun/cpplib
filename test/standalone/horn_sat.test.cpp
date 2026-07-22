// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/constraints/horn_sat.hpp"

void test_satisfiable(){
    const std::vector<HornClause> clauses = {
        {{}, 0},
        {{0}, 1},
        {{1}, 2},
        {{0, 0}, 1},
    };
    const auto result = horn_sat(4, clauses);
    assert(result.satisfiable);
    assert((result.minimum_model == std::vector<char>{1, 1, 1, 0}));
}

void test_unsatisfiable(){
    const std::vector<HornClause> clauses = {
        {{}, 0},
        {{0}, -1},
    };
    const auto result = horn_sat(1, clauses);
    assert(!result.satisfiable);
    assert(result.minimum_model.empty());
}

void test_tautology_and_empty(){
    const auto tautology = horn_sat(1, {{{0}, 0}});
    assert(tautology.satisfiable);
    assert((tautology.minimum_model == std::vector<char>{0}));
    const auto empty = horn_sat(0, {});
    assert(empty.satisfiable);
    assert(empty.minimum_model.empty());
    assert(!horn_sat(0, {{{}, -1}}).satisfiable);
}

void test_validation(){
    bool thrown = false;
    try{
        (void)horn_sat(-1, {});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)horn_sat(1, {{{1}, -1}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)horn_sat(1, {{{}, 1}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int case_count;
    if(std::cin >> case_count){
        while(case_count--){
            int variable_count, clause_count;
            std::cin >> variable_count >> clause_count;
            std::vector<HornClause> clauses(
                static_cast<std::size_t>(clause_count)
            );
            for(HornClause& clause: clauses){
                int negative_count;
                std::cin >> clause.positive_variable >> negative_count;
                clause.negative_variables.resize(
                    static_cast<std::size_t>(negative_count)
                );
                for(int& variable: clause.negative_variables){
                    std::cin >> variable;
                }
            }
            const auto result = horn_sat(variable_count, clauses);
            std::cout << result.satisfiable;
            for(char value: result.minimum_model){
                std::cout << ' ' << static_cast<int>(value);
            }
            std::cout << '\n';
        }
        return 0;
    }
    test_satisfiable();
    test_unsatisfiable();
    test_tautology_and_empty();
    test_validation();
}
