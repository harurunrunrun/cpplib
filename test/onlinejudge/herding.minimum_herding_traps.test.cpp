// competitive-verifier: PROBLEM https://www.spoj.com/problems/HERDING/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/minimum_herding_traps.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int row_count, column_count;
    if(!(std::cin >> row_count >> column_count)) return 0;
    std::vector<std::string> directions(static_cast<std::size_t>(row_count));
    for(auto& row: directions) std::cin >> row;
    std::cout << minimum_herding_traps(directions) << '\n';
}
