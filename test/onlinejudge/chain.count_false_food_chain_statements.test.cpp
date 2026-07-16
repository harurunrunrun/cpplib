// competitive-verifier: PROBLEM https://www.spoj.com/problems/CHAIN/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/count_inconsistent_modulo_three_constraints.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int animal_count, statement_count;
        std::cin >> animal_count >> statement_count;
        std::vector<ModularDifferenceConstraint> constraints;
        constraints.reserve(static_cast<std::size_t>(statement_count));
        for(int index = 0; index < statement_count; index++){
            int relation;
            ModularDifferenceConstraint constraint;
            std::cin >> relation >> constraint.left >> constraint.right;
            constraint.difference = relation == 1 ? 0 : 2;
            --constraint.left;
            --constraint.right;
            constraints.push_back(constraint);
        }
        std::cout << count_inconsistent_modulo_three_constraints(
            animal_count,
            constraints
        ) << '\n';
    }
}
