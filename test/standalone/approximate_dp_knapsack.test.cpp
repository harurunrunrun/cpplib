// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/dp/knapsack.hpp"
#include "../../src/approximate/dp/subset_sum.hpp"

namespace {

using approximate::dp::KnapsackResult;
using approximate::dp::SubsetSumResult;

[[nodiscard]] bool valid_knapsack_result(
    const KnapsackResult& result,
    const std::vector<std::uint64_t>& weights,
    const std::vector<std::uint64_t>& values,
    std::uint64_t capacity
){
    if(!std::is_sorted(result.selected_indices.begin(), result.selected_indices.end())) return false;
    if(std::adjacent_find(result.selected_indices.begin(), result.selected_indices.end()) !=
       result.selected_indices.end()) return false;
    __uint128_t weight = 0;
    __uint128_t value = 0;
    for(const std::size_t index : result.selected_indices){
        if(index >= weights.size()) return false;
        weight += weights[index];
        value += values[index];
    }
    return weight <= capacity &&
           weight == result.total_weight &&
           value == result.total_value;
}

[[nodiscard]] bool valid_subset_result(
    const SubsetSumResult& result,
    const std::vector<std::uint64_t>& values,
    std::uint64_t capacity
){
    if(!std::is_sorted(result.selected_indices.begin(), result.selected_indices.end())) return false;
    if(std::adjacent_find(result.selected_indices.begin(), result.selected_indices.end()) !=
       result.selected_indices.end()) return false;
    __uint128_t sum = 0;
    for(const std::size_t index : result.selected_indices){
        if(index >= values.size()) return false;
        sum += values[index];
    }
    return sum <= capacity && sum == result.sum;
}

[[nodiscard]] std::uint64_t exact_knapsack(
    const std::vector<std::uint64_t>& weights,
    const std::vector<std::uint64_t>& values,
    std::size_t capacity
){
    std::vector<std::uint64_t> dp(capacity + 1, 0);
    for(std::size_t item = 0; item < weights.size(); ++item){
        if(weights[item] > capacity) continue;
        const std::size_t weight = static_cast<std::size_t>(weights[item]);
        for(std::size_t current = capacity + 1; current-- > weight;){
            if(values[item] > std::numeric_limits<std::uint64_t>::max() - dp[current - weight]){
                throw std::overflow_error("test optimum overflow");
            }
            dp[current] = std::max(dp[current], dp[current - weight] + values[item]);
        }
    }
    return *std::max_element(dp.begin(), dp.end());
}

[[nodiscard]] std::uint64_t exact_subset_sum(
    const std::vector<std::uint64_t>& values,
    std::size_t capacity
){
    std::vector<unsigned char> reachable(capacity + 1, 0);
    reachable[0] = 1;
    for(const std::uint64_t raw_value : values){
        if(raw_value > capacity) continue;
        const std::size_t value = static_cast<std::size_t>(raw_value);
        for(std::size_t sum = capacity + 1; sum-- > value;){
            if(reachable[sum - value] != 0) reachable[sum] = 1;
        }
    }
    for(std::size_t sum = capacity + 1; sum-- > 0;){
        if(reachable[sum] != 0) return static_cast<std::uint64_t>(sum);
    }
    return 0;
}

[[nodiscard]] bool meets_ratio(
    std::uint64_t actual,
    std::uint64_t optimum,
    long double ratio
){
    return static_cast<long double>(actual) + 1.0e-12L >=
           ratio * static_cast<long double>(optimum);
}

}  // namespace

int main(){
    using namespace approximate::dp;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const std::vector<std::uint64_t> fixed_weights{4, 3, 2};
    const std::vector<std::uint64_t> fixed_values{10, 9, 8};
    if(greedy_value_knapsack(fixed_weights, fixed_values, 5).selected_indices !=
       std::vector<std::size_t>{0}) return 2;
    if(greedy_weight_knapsack(fixed_weights, fixed_values, 5).selected_indices !=
       (std::vector<std::size_t>{1, 2})) return 3;
    if(greedy_ratio_knapsack(fixed_weights, fixed_values, 5).selected_indices !=
       (std::vector<std::size_t>{1, 2})) return 4;
    if(modified_greedy_knapsack(
           std::vector<int>{5, 6}, std::vector<int>{10, 12}, 10
       ).selected_indices != std::vector<std::size_t>{1}) return 5;
    if(modified_greedy_knapsack(
           std::vector<int>{0, 5, 6}, std::vector<int>{7, 10, 12}, 10
       ).selected_indices != (std::vector<std::size_t>{0, 2})) return 6;

    bool rejected = false;
    try{
        static_cast<void>(greedy_ratio_knapsack(
            std::vector<int>{1}, std::vector<int>{1, 2}, 1
        ));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 7;
    rejected = false;
    try{
        static_cast<void>(greedy_value_knapsack(
            std::vector<int>{-1}, std::vector<int>{1}, 1
        ));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 8;
    rejected = false;
    try{
        static_cast<void>(value_scaling_knapsack_fptas(
            std::vector<int>{1}, std::vector<int>{1}, 1,
            std::numeric_limits<long double>::quiet_NaN()
        ));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 9;
    rejected = false;
    try{
        static_cast<void>(subset_sum_trimming_fptas(std::vector<int>{1}, 1, 1.0L));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 10;
    const SubsetSumResult maximum_sum = sparse_subset_sum(
        std::vector<std::uint64_t>{std::numeric_limits<std::uint64_t>::max()},
        std::numeric_limits<std::uint64_t>::max()
    );
    if(maximum_sum.sum != std::numeric_limits<std::uint64_t>::max() ||
       maximum_sum.selected_indices != std::vector<std::size_t>{0}) return 11;
    rejected = false;
    try{
        static_cast<void>(value_scaling_knapsack_fptas(
            std::vector<std::uint64_t>{0, 0},
            std::vector<std::uint64_t>{std::numeric_limits<std::uint64_t>::max(), 1},
            std::uint64_t{0},
            0.5L
        ));
    }catch(const std::overflow_error&){
        rejected = true;
    }
    if(!rejected) return 12;

    std::size_t knapsack_cases = 0;
    std::cin >> knapsack_cases;
    for(std::size_t case_index = 0; case_index < knapsack_cases; ++case_index){
        std::size_t size = 0;
        std::size_t capacity = 0;
        long double epsilon = 0;
        std::cin >> size >> capacity >> epsilon;
        std::vector<std::uint64_t> weights(size);
        std::vector<std::uint64_t> values(size);
        for(std::uint64_t& weight : weights) std::cin >> weight;
        for(std::uint64_t& value : values) std::cin >> value;

        const std::uint64_t optimum = exact_knapsack(weights, values, capacity);
        const KnapsackResult ratio = greedy_ratio_knapsack(weights, values, capacity);
        const KnapsackResult by_value = greedy_value_knapsack(weights, values, capacity);
        const KnapsackResult by_weight = greedy_weight_knapsack(weights, values, capacity);
        const KnapsackResult modified = modified_greedy_knapsack(weights, values, capacity);
        const KnapsackResult fptas = value_scaling_knapsack_fptas(
            weights, values, capacity, epsilon
        );
        if(!valid_knapsack_result(ratio, weights, values, capacity) ||
           !valid_knapsack_result(by_value, weights, values, capacity) ||
           !valid_knapsack_result(by_weight, weights, values, capacity) ||
           !valid_knapsack_result(modified, weights, values, capacity) ||
           !valid_knapsack_result(fptas, weights, values, capacity)) return 20;
        if(static_cast<__uint128_t>(modified.total_value) * 2U < optimum) return 21;
        if(!meets_ratio(fptas.total_value, optimum, 1.0L - epsilon)) return 22;
        std::cout << "K " << optimum << '\n';
    }

    std::size_t subset_cases = 0;
    std::cin >> subset_cases;
    for(std::size_t case_index = 0; case_index < subset_cases; ++case_index){
        std::size_t size = 0;
        std::size_t capacity = 0;
        long double epsilon = 0;
        std::cin >> size >> capacity >> epsilon;
        std::vector<std::uint64_t> values(size);
        for(std::uint64_t& value : values) std::cin >> value;
        const std::uint64_t optimum = exact_subset_sum(values, capacity);
        const SubsetSumResult exact = sparse_subset_sum(values, capacity);
        const SubsetSumResult trimmed = subset_sum_trimming_fptas(values, capacity, epsilon);
        if(!valid_subset_result(exact, values, capacity) ||
           !valid_subset_result(trimmed, values, capacity)) return 30;
        if(exact.sum != optimum) return 31;
        if(!meets_ratio(trimmed.sum, optimum, 1.0L - epsilon)) return 32;
        std::cout << "S " << optimum << '\n';
    }
}
