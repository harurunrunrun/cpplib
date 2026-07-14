// competitive-verifier: PROBLEM https://www.spoj.com/problems/CHAIN/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/count_false_food_chain_statements.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int animal_count, statement_count;
        std::cin >> animal_count >> statement_count;
        std::vector<FoodChainStatement> statements;
        statements.reserve(static_cast<std::size_t>(statement_count));
        for(int index = 0; index < statement_count; index++){
            FoodChainStatement statement;
            std::cin >> statement.relation >> statement.left >> statement.right;
            statement.left--;
            statement.right--;
            statements.push_back(statement);
        }
        std::cout << count_false_food_chain_statements(animal_count, statements) << '\n';
    }
}
