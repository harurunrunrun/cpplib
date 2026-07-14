// competitive-verifier: PROBLEM https://www.spoj.com/problems/COCONUTS/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/minimum_opinion_disagreement.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int guard_count, friendship_count;
    while(std::cin >> guard_count >> friendship_count &&
          (guard_count != 0 || friendship_count != 0)){
        std::vector<bool> beliefs(static_cast<std::size_t>(guard_count));
        for(int guard = 0; guard < guard_count; guard++){
            int belief;
            std::cin >> belief;
            beliefs[static_cast<std::size_t>(guard)] = belief != 0;
        }
        std::vector<std::pair<int, int>> friendships;
        friendships.reserve(static_cast<std::size_t>(friendship_count));
        for(int index = 0; index < friendship_count; index++){
            int left, right;
            std::cin >> left >> right;
            friendships.emplace_back(left - 1, right - 1);
        }
        std::cout << minimum_opinion_disagreement(beliefs, friendships) << '\n';
    }
}
