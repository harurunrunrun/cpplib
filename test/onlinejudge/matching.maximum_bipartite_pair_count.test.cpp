// competitive-verifier: PROBLEM https://www.spoj.com/problems/MATCHING/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/matching/bipartite/maximum_bipartite_pair_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int left_size, right_size, pair_count;
    if(!(std::cin >> left_size >> right_size >> pair_count)) return 0;
    std::vector<std::pair<int, int>> allowed_pairs;
    allowed_pairs.reserve(static_cast<std::size_t>(pair_count));
    for(int index = 0; index < pair_count; index++){
        int left, right;
        std::cin >> left >> right;
        allowed_pairs.emplace_back(left - 1, right - 1);
    }
    std::cout << maximum_bipartite_pair_count(
        left_size, right_size, allowed_pairs
    ) << '\n';
}
