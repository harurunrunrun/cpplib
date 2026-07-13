// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/assignment

#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/assignment.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<std::vector<long long>> cost(
        static_cast<std::size_t>(n),
        std::vector<long long>(static_cast<std::size_t>(n))
    );
    for(auto& row: cost){
        for(long long& value: row) std::cin >> value;
    }
    const auto result = minimum_cost_assignment(cost);
    std::cout << result.cost << '\n';
    for(int row = 0; row < n; ++row){
        if(row != 0) std::cout << ' ';
        std::cout << result.row_to_column[static_cast<std::size_t>(row)];
    }
    std::cout << '\n';
}
