// competitive-verifier: PROBLEM https://www.spoj.com/problems/COCONUTS/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/minimum_binary_labeling_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int variable_count, edge_count;
    while(std::cin >> variable_count >> edge_count &&
          (variable_count != 0 || edge_count != 0)){
        std::vector<BinaryLabelUnaryCost> unary_costs(
            static_cast<std::size_t>(variable_count)
        );
        for(auto& cost: unary_costs){
            int preferred_label;
            std::cin >> preferred_label;
            cost = preferred_label != 0
                ? BinaryLabelUnaryCost{1, 0}
                : BinaryLabelUnaryCost{0, 1};
        }
        std::vector<BinaryLabelPairwiseCost> pairwise_costs;
        pairwise_costs.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- > 0){
            int left, right;
            std::cin >> left >> right;
            pairwise_costs.push_back({left - 1, right - 1, 1});
        }
        std::cout << minimum_binary_labeling_cost(
            unary_costs, pairwise_costs
        ) << '\n';
    }
}
