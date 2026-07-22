// competitive-verifier: PROBLEM https://www.spoj.com/problems/KATHTHI/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/minimum_grid_character_changes.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        int row_count, column_count;
        std::cin >> row_count >> column_count;
        std::vector<std::string> grid(static_cast<std::size_t>(row_count));
        for(auto& row: grid) std::cin >> row;

        std::cout << minimum_grid_character_changes(grid) << '\n';
    }
}
