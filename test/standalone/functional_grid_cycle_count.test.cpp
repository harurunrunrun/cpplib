// competitive-verifier: STANDALONE

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/traversal/functional_grid_cycle_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int row_count, column_count;
        std::cin >> row_count >> column_count;
        std::vector<std::string> directions(
            static_cast<std::size_t>(row_count)
        );
        for(auto& row: directions) std::cin >> row;
        std::cout << functional_grid_cycle_count(directions) << '\n';
    }
}
