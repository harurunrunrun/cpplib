// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc317/tasks/abc317_g

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/specialized/rearrange_rows_into_permutation_columns.hpp"

int main(){
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> matrix(
        static_cast<std::size_t>(n),
        std::vector<int>(static_cast<std::size_t>(m))
    );
    for(auto& row: matrix){
        for(int& value: row){
            std::cin >> value;
            --value;
        }
    }
    const auto answer = rearrange_rows_into_permutation_columns(matrix);
    std::cout << "Yes\n";
    for(const auto& row: answer){
        for(int column = 0; column < m; ++column){
            if(column != 0) std::cout << ' ';
            std::cout << row[static_cast<std::size_t>(column)] + 1;
        }
        std::cout << '\n';
    }
}
