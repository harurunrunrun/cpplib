// competitive-verifier: PROBLEM https://www.spoj.com/problems/FOXLINGS/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/sparse_undirected_component_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        long long foxling_count;
        int friendship_count;
        std::cin >> foxling_count >> friendship_count;
        std::vector<std::pair<long long, long long>> friendships;
        friendships.reserve(static_cast<std::size_t>(friendship_count));
        for(int index = 0; index < friendship_count; index++){
            long long left, right;
            std::cin >> left >> right;
            friendships.emplace_back(left - 1, right - 1);
        }
        std::cout << sparse_undirected_component_count(foxling_count, friendships) << '\n';
    }
}
