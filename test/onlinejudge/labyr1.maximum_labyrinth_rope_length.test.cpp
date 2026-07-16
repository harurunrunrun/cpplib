// competitive-verifier: PROBLEM https://www.spoj.com/problems/LABYR1/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/grid_forest_diameter.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int column_count, row_count;
        std::cin >> column_count >> row_count;
        std::vector<std::string> labyrinth(static_cast<std::size_t>(row_count));
        for(auto& row: labyrinth) std::cin >> row;
        std::cout << "Maximum rope length is "
                  << grid_forest_diameter(labyrinth) << ".\n";
    }
}
