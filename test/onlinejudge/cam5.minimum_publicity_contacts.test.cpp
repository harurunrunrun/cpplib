// competitive-verifier: PROBLEM https://www.spoj.com/problems/CAM5/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/undirected_component_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int peer_count, friendship_count;
        std::cin >> peer_count >> friendship_count;
        std::vector<std::pair<int, int>> friendships;
        friendships.reserve(static_cast<std::size_t>(friendship_count));
        for(int index = 0; index < friendship_count; index++){
            int left, right;
            std::cin >> left >> right;
            friendships.emplace_back(left, right);
        }
        std::cout << undirected_component_count(peer_count, friendships) << '\n';
    }
}
