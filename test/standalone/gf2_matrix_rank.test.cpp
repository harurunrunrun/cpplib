// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/gf2_matrix_rank.hpp"

void self_test(){
    assert(gf2_matrix_rank({"10", "01"}) == 2);
    assert(gf2_matrix_rank({"11", "11"}) == 1);
    assert(gf2_matrix_rank({"000", "000"}) == 0);
    assert(gf2_matrix_rank({}, 7) == 0);
}

int main(){
    self_test();

    int row_count, column_count;
    if(!(std::cin >> row_count >> column_count)) return 0;
    std::vector<std::string> matrix(static_cast<std::size_t>(row_count));
    for(std::string& row: matrix) std::cin >> row;
    std::cout << gf2_matrix_rank(
        matrix, static_cast<std::size_t>(column_count)
    ) << '\n';
}
