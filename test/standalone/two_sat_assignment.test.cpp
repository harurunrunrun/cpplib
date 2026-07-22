// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/constraints/two_sat_assignment.hpp"

struct Clause{
    int left_variable;
    bool left_value;
    int right_variable;
    bool right_value;
};

void self_test(){
    TwoSat satisfiable(2);
    satisfiable.add_clause(0, true, 1, false);
    satisfiable.add_clause(0, false, 1, true);
    const auto assignment = two_sat_assignment(satisfiable);
    assert(assignment);
    assert(assignment->size() == 2);
    assert(
        ((*assignment)[0] == 1 || (*assignment)[1] == 0) &&
        ((*assignment)[0] == 0 || (*assignment)[1] == 1)
    );

    TwoSat impossible(1);
    impossible.add_clause(0, true, 0, true);
    impossible.add_clause(0, false, 0, false);
    assert(!two_sat_assignment(impossible));

    TwoSat empty(0);
    const auto empty_assignment = two_sat_assignment(empty);
    assert(empty_assignment);
    assert(empty_assignment->empty());
}

int main(){
    self_test();

    int variable_count, clause_count;
    if(!(std::cin >> variable_count >> clause_count)) return 0;
    TwoSat solver(variable_count);
    std::vector<Clause> clauses;
    clauses.reserve(static_cast<std::size_t>(clause_count));
    for(int clause_index = 0; clause_index < clause_count; ++clause_index){
        Clause clause{};
        int left_value, right_value;
        std::cin >> clause.left_variable >> left_value
                 >> clause.right_variable >> right_value;
        clause.left_value = left_value != 0;
        clause.right_value = right_value != 0;
        clauses.push_back(clause);
        solver.add_clause(
            clause.left_variable,
            clause.left_value,
            clause.right_variable,
            clause.right_value
        );
    }

    const auto assignment = two_sat_assignment(solver);
    if(!assignment){
        std::cout << 0 << '\n';
        return 0;
    }
    assert(assignment->size() == static_cast<std::size_t>(variable_count));
    for(const Clause& clause: clauses){
        assert(
            ((*assignment)[static_cast<std::size_t>(clause.left_variable)] != 0) ==
                clause.left_value ||
            ((*assignment)[static_cast<std::size_t>(clause.right_variable)] != 0) ==
                clause.right_value
        );
    }
    std::cout << 1 << '\n';
}
