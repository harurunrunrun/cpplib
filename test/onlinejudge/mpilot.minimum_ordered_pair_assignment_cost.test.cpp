// competitive-verifier: PROBLEM https://www.spoj.com/problems/MPILOT/
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/minimum_ordered_pair_assignment_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int item_count;
    if(!(std::cin >> item_count)) return 0;
    std::vector<OrderedPairAssignmentCosts> ordered_costs(
        static_cast<std::size_t>(item_count)
    );
    for(auto& costs: ordered_costs){
        long long closing_cost, opening_cost;
        std::cin >> closing_cost >> opening_cost;
        costs = {opening_cost, closing_cost};
    }
    std::cout << minimum_ordered_pair_assignment_cost(ordered_costs) << '\n';
    return 0;
}
