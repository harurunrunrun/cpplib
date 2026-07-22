// competitive-verifier: PROBLEM https://www.spoj.com/problems/BUGLIFE/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/constraints/is_bipartite.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int scenario_count;
    if(!(std::cin >> scenario_count)) return 0;
    for(int scenario = 1; scenario <= scenario_count; scenario++){
        int bug_count, interaction_count;
        std::cin >> bug_count >> interaction_count;
        std::vector<std::pair<int, int>> interactions;
        interactions.reserve(static_cast<std::size_t>(interaction_count));
        for(int index = 0; index < interaction_count; index++){
            int left, right;
            std::cin >> left >> right;
            interactions.emplace_back(left - 1, right - 1);
        }
        const bool consistent = is_bipartite(
            bug_count, interactions
        );
        std::cout << "Scenario #" << scenario << ":\n";
        std::cout << (consistent
            ? "No suspicious bugs found!\n"
            : "Suspicious bugs found!\n");
    }
}
