// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc259/tasks/abc259_g

#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/maximum_grid_row_column_selection_score.hpp"

int main(){
    int height, width;
    std::cin >> height >> width;
    std::vector<std::vector<long long>> grid(
        static_cast<std::size_t>(height),
        std::vector<long long>(static_cast<std::size_t>(width))
    );
    for(auto& row: grid){
        for(long long& value: row) std::cin >> value;
    }
    std::cout << maximum_grid_row_column_selection_score(grid) << '\n';
}
