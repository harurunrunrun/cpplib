// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/minimum_binary_labeling_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int variable_count, pairwise_count;
        std::cin >> variable_count >> pairwise_count;
        std::vector<BinaryLabelUnaryCost> unary_costs(
            static_cast<std::size_t>(variable_count)
        );
        for(auto& cost: unary_costs){
            std::cin >> cost.cost_if_zero >> cost.cost_if_one;
        }
        std::vector<BinaryLabelPairwiseCost> pairwise_costs(
            static_cast<std::size_t>(pairwise_count)
        );
        for(auto& cost: pairwise_costs){
            std::cin >> cost.left >> cost.right >> cost.different_cost;
        }
        std::cout << minimum_binary_labeling_cost(
            unary_costs, pairwise_costs
        ) << '\n';
    }
}
