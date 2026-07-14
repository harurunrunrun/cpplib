// competitive-verifier: PROBLEM https://www.spoj.com/problems/BYTESE1/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/weighted_grid_escape.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int rows, columns;
        std::cin >> rows >> columns;
        std::vector<std::vector<long long>> cost(
            static_cast<std::size_t>(rows),
            std::vector<long long>(static_cast<std::size_t>(columns))
        );
        for(auto& row: cost) for(long long& value: row) std::cin >> value;
        int target_row, target_column;
        long long budget;
        std::cin >> target_row >> target_column >> budget;
        const auto remaining = weighted_grid_escape(
            cost, target_row - 1, target_column - 1, budget
        );
        if(remaining){
            std::cout << "YES\n" << *remaining << '\n';
        }else{
            std::cout << "NO\n";
        }
    }
}
