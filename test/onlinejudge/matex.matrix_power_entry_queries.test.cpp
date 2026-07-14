// competitive-verifier: PROBLEM https://www.spoj.com/problems/MATEX/
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include "../../src/algorithm/math/matrix_power_entry_queries.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t width;
    if(!(std::cin >> width)) return 0;
    math::MatrixPowerEntryQueries<>::input_matrix_type matrix(
        width,
        std::vector<std::int64_t>(width)
    );
    for(auto& row: matrix){
        for(auto& value: row) std::cin >> value;
    }
    math::MatrixPowerEntryQueries<> queries(matrix);
    std::size_t row, column;
    std::uint64_t exponent;
    while(std::cin >> row >> column >> exponent){
        std::cout << queries.entry(row - 1, column - 1, exponent) << '\n';
    }
}
