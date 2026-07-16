#pragma once

#include <limits>
#include <stdexcept>
#include <vector>

#include "../matching/dinic.hpp"

struct BinaryLabelUnaryCost{
    long long cost_if_zero;
    long long cost_if_one;
};

struct BinaryLabelPairwiseCost{
    int left;
    int right;
    long long different_cost;
};

inline long long minimum_binary_labeling_cost(
    const std::vector<BinaryLabelUnaryCost>& unary_costs,
    const std::vector<BinaryLabelPairwiseCost>& pairwise_costs
){
    if(unary_costs.size() >
       static_cast<std::size_t>(std::numeric_limits<int>::max() - 2))[[unlikely]]{
        throw std::length_error("minimum_binary_labeling_cost too many variables");
    }
    const int variable_count = static_cast<int>(unary_costs.size());
    const int source = variable_count;
    const int sink = variable_count + 1;
    Dinic<__int128_t> flow(variable_count + 2);

    for(int variable = 0; variable < variable_count; ++variable){
        const auto& cost = unary_costs[static_cast<std::size_t>(variable)];
        if(cost.cost_if_zero < 0 || cost.cost_if_one < 0)[[unlikely]]{
            throw std::invalid_argument(
                "minimum_binary_labeling_cost negative unary cost"
            );
        }
        flow.add_edge(source, variable, cost.cost_if_zero);
        flow.add_edge(variable, sink, cost.cost_if_one);
    }
    for(const auto& cost: pairwise_costs){
        if(cost.left < 0 || variable_count <= cost.left ||
           cost.right < 0 || variable_count <= cost.right)[[unlikely]]{
            throw std::out_of_range(
                "minimum_binary_labeling_cost endpoint out of range"
            );
        }
        if(cost.different_cost < 0)[[unlikely]]{
            throw std::invalid_argument(
                "minimum_binary_labeling_cost negative pairwise cost"
            );
        }
        if(cost.left == cost.right) continue;
        flow.add_edge(cost.left, cost.right, cost.different_cost);
        flow.add_edge(cost.right, cost.left, cost.different_cost);
    }

    const __int128_t answer = flow.max_flow(source, sink);
    if(answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error("minimum_binary_labeling_cost overflow");
    }
    return static_cast<long long>(answer);
}
