// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/matching/maximum_grid_row_column_selection_score.hpp"

void self_test(){
    assert(maximum_grid_row_column_selection_score({}) == 0);
    assert(maximum_grid_row_column_selection_score({{}}) == 0);
    assert(maximum_grid_row_column_selection_score({{7}}) == 7);
    assert(maximum_grid_row_column_selection_score({{-9}}) == 0);
    assert(maximum_grid_row_column_selection_score({{1, 2, 3}}) == 6);

    bool thrown = false;
    try{
        (void)maximum_grid_row_column_selection_score({{1}, {2, 3}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int height, width;
    if(!(std::cin >> height >> width)) return 0;
    std::vector<std::vector<long long>> grid(
        static_cast<std::size_t>(height),
        std::vector<long long>(static_cast<std::size_t>(width))
    );
    for(auto& row: grid){
        for(long long& value: row) std::cin >> value;
    }
    std::cout << maximum_grid_row_column_selection_score(grid) << '\n';
}
