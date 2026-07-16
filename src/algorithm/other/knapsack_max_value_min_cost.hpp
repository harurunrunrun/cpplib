#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

struct KnapsackItem {
    int cost;
    int value;
};

struct KnapsackOptimum {
    int minimum_cost;
    int maximum_value;
};

inline KnapsackOptimum knapsack_max_value_min_cost(
    int capacity,
    const std::vector<KnapsackItem>& items
){
    if(capacity < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative capacity (knapsack_max_value_min_cost)."
        );
    }
    std::vector<int> dp(static_cast<std::size_t>(capacity + 1));
    for(const auto& item: items){
        if(item.cost < 0 || item.value < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative item cost or value "
                "(knapsack_max_value_min_cost)."
            );
        }
        if(item.cost > capacity) continue;
        for(int allowed_capacity = capacity;
            allowed_capacity >= item.cost;
            --allowed_capacity){
            dp[static_cast<std::size_t>(allowed_capacity)] = std::max(
                dp[static_cast<std::size_t>(allowed_capacity)],
                dp[static_cast<std::size_t>(
                    allowed_capacity - item.cost
                )] + item.value
            );
        }
    }
    const int best_value = *std::max_element(dp.begin(), dp.end());
    const int minimum_cost = static_cast<int>(
        std::find(dp.begin(), dp.end(), best_value) - dp.begin()
    );
    return {minimum_cost, best_value};
}
