#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace approximate::dp {

struct KnapsackResult {
    std::vector<std::size_t> selected_indices;
    std::uint64_t total_weight = 0;
    std::uint64_t total_value = 0;
};

namespace detail {

template<class Integer>
[[nodiscard]] std::uint64_t nonnegative_integer(Integer value, const char* message){
    static_assert(std::is_integral_v<Integer> && !std::is_same_v<Integer, bool>);
    static_assert(sizeof(Integer) <= sizeof(std::uint64_t));
    if constexpr(std::is_signed_v<Integer>){
        if(value < 0) throw std::invalid_argument(message);
    }
    return static_cast<std::uint64_t>(value);
}

[[nodiscard]] inline std::uint64_t checked_add(
    std::uint64_t left,
    std::uint64_t right,
    const char* message
){
    if(right > std::numeric_limits<std::uint64_t>::max() - left){
        throw std::overflow_error(message);
    }
    return left + right;
}

struct KnapsackItem {
    std::uint64_t weight = 0;
    std::uint64_t value = 0;
    std::size_t index = 0;
};

template<class Weight, class Value, class Capacity>
[[nodiscard]] std::pair<std::vector<KnapsackItem>, std::uint64_t> normalize_knapsack(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity
){
    static_assert(std::is_integral_v<Weight> && !std::is_same_v<Weight, bool>);
    static_assert(std::is_integral_v<Value> && !std::is_same_v<Value, bool>);
    static_assert(std::is_integral_v<Capacity> && !std::is_same_v<Capacity, bool>);
    if(weights.size() != values.size()){
        throw std::invalid_argument("weights and values must have the same size");
    }
    const std::uint64_t normalized_capacity = nonnegative_integer(
        capacity,
        "capacity must be nonnegative"
    );
    std::vector<KnapsackItem> items;
    items.reserve(weights.size());
    for(std::size_t index = 0; index < weights.size(); ++index){
        items.push_back({
            nonnegative_integer(weights[index], "weights must be nonnegative"),
            nonnegative_integer(values[index], "values must be nonnegative"),
            index
        });
    }
    return {std::move(items), normalized_capacity};
}

[[nodiscard]] inline KnapsackResult make_knapsack_result(
    const std::vector<KnapsackItem>& items,
    std::vector<bool> selected
){
    KnapsackResult result;
    for(const KnapsackItem& item : items){
        if(!selected[item.index]) continue;
        result.selected_indices.push_back(item.index);
        result.total_weight = checked_add(
            result.total_weight,
            item.weight,
            "the selected total weight does not fit in uint64_t"
        );
        result.total_value = checked_add(
            result.total_value,
            item.value,
            "the selected total value does not fit in uint64_t"
        );
    }
    return result;
}

template<class Less>
[[nodiscard]] KnapsackResult greedy_knapsack_impl(
    const std::vector<KnapsackItem>& items,
    std::uint64_t capacity,
    Less less
){
    std::vector<std::size_t> order;
    order.reserve(items.size());
    for(std::size_t index = 0; index < items.size(); ++index){
        if(items[index].value != 0) order.push_back(index);
    }
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        return less(items[left], items[right]);
    });
    std::vector<bool> selected(items.size(), false);
    std::uint64_t used = 0;
    for(const std::size_t index : order){
        if(items[index].weight > capacity - used) continue;
        selected[index] = true;
        used += items[index].weight;
    }
    return make_knapsack_result(items, std::move(selected));
}

struct RatioLess {
    [[nodiscard]] bool operator()(const KnapsackItem& left, const KnapsackItem& right) const {
        if(left.weight == 0 || right.weight == 0){
            if(left.weight == 0 && right.weight != 0) return true;
            if(left.weight != 0 && right.weight == 0) return false;
        }
        const __uint128_t lhs = static_cast<__uint128_t>(left.value) * right.weight;
        const __uint128_t rhs = static_cast<__uint128_t>(right.value) * left.weight;
        if(lhs != rhs) return lhs > rhs;
        return left.index < right.index;
    }
};

}  // namespace detail

template<class Weight, class Value, class Capacity>
[[nodiscard]] KnapsackResult greedy_ratio_knapsack(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity
){
    auto [items, normalized_capacity] = detail::normalize_knapsack(weights, values, capacity);
    return detail::greedy_knapsack_impl(items, normalized_capacity, detail::RatioLess{});
}

template<class Weight, class Value, class Capacity>
[[nodiscard]] KnapsackResult greedy_value_knapsack(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity
){
    auto [items, normalized_capacity] = detail::normalize_knapsack(weights, values, capacity);
    return detail::greedy_knapsack_impl(
        items,
        normalized_capacity,
        [](const detail::KnapsackItem& left, const detail::KnapsackItem& right){
            if(left.value != right.value) return left.value > right.value;
            return left.index < right.index;
        }
    );
}

template<class Weight, class Value, class Capacity>
[[nodiscard]] KnapsackResult greedy_weight_knapsack(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity
){
    auto [items, normalized_capacity] = detail::normalize_knapsack(weights, values, capacity);
    return detail::greedy_knapsack_impl(
        items,
        normalized_capacity,
        [](const detail::KnapsackItem& left, const detail::KnapsackItem& right){
            if(left.weight != right.weight) return left.weight < right.weight;
            return left.index < right.index;
        }
    );
}

template<class Weight, class Value, class Capacity>
[[nodiscard]] KnapsackResult modified_greedy_knapsack(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity
){
    auto [items, normalized_capacity] = detail::normalize_knapsack(weights, values, capacity);
    const KnapsackResult ratio = detail::greedy_knapsack_impl(
        items,
        normalized_capacity,
        detail::RatioLess{}
    );

    std::vector<bool> selected(items.size(), false);
    std::size_t best = items.size();
    for(const detail::KnapsackItem& item : items){
        if(item.weight == 0 && item.value != 0) selected[item.index] = true;
        if(item.weight == 0 || item.weight > normalized_capacity || item.value == 0) continue;
        if(best == items.size() || item.value > items[best].value ||
           (item.value == items[best].value && item.index < items[best].index)){
            best = item.index;
        }
    }
    if(best != items.size()) selected[best] = true;
    const KnapsackResult single = detail::make_knapsack_result(items, std::move(selected));
    return ratio.total_value >= single.total_value ? ratio : single;
}

template<class Weight, class Value, class Capacity>
[[nodiscard]] KnapsackResult value_scaling_knapsack_fptas(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity,
    long double epsilon
){
    if(!std::isfinite(epsilon) || !(epsilon > 0.0L && epsilon < 1.0L)){
        throw std::invalid_argument("epsilon must be finite and in (0, 1)");
    }
    auto [items, normalized_capacity] = detail::normalize_knapsack(weights, values, capacity);
    std::vector<bool> selected(items.size(), false);
    std::vector<std::size_t> candidates;
    std::uint64_t maximum_value = 0;
    for(const detail::KnapsackItem& item : items){
        if(item.weight == 0 && item.value != 0) selected[item.index] = true;
        if(item.weight == 0 || item.weight > normalized_capacity || item.value == 0) continue;
        candidates.push_back(item.index);
        maximum_value = std::max(maximum_value, item.value);
    }
    if(candidates.empty()) return detail::make_knapsack_result(items, std::move(selected));

    long double scale = epsilon * static_cast<long double>(maximum_value) /
                        static_cast<long double>(candidates.size());
    if(scale < 1.0L) scale = 1.0L;

    struct ScaledItem {
        std::size_t index = 0;
        std::size_t value = 0;
    };
    std::vector<ScaledItem> scaled_items;
    scaled_items.reserve(candidates.size());
    std::size_t scaled_sum = 0;
    for(const std::size_t index : candidates){
        const long double quotient = std::floor(
            static_cast<long double>(items[index].value) / scale
        );
        if(!(quotient >= 0.0L) ||
           quotient > static_cast<long double>(std::numeric_limits<std::size_t>::max())){
            throw std::length_error("the scaled DP index does not fit in size_t");
        }
        const std::size_t scaled_value = static_cast<std::size_t>(quotient);
        if(scaled_value == 0) continue;
        if(scaled_value > std::numeric_limits<std::size_t>::max() - scaled_sum){
            throw std::length_error("the scaled DP range does not fit in size_t");
        }
        scaled_sum += scaled_value;
        scaled_items.push_back({index, scaled_value});
    }
    if(scaled_items.empty()) return detail::make_knapsack_result(items, std::move(selected));
    if(scaled_sum == std::numeric_limits<std::size_t>::max()){
        throw std::length_error("the scaled DP range is too large");
    }

    const std::size_t state_count = scaled_sum + 1;
    const std::size_t word_count = state_count / 64U +
                                   static_cast<std::size_t>(state_count % 64U != 0);
    if(word_count != 0 && scaled_items.size() >
       std::numeric_limits<std::size_t>::max() / word_count){
        throw std::length_error("the reconstruction table is too large");
    }
    std::vector<std::uint64_t> minimum_weight(state_count, 0);
    std::vector<unsigned char> reachable(state_count, 0);
    reachable[0] = 1;
    std::vector<std::vector<std::uint64_t>> take(
        scaled_items.size(),
        std::vector<std::uint64_t>(word_count, 0)
    );

    std::size_t current_sum = 0;
    for(std::size_t row = 0; row < scaled_items.size(); ++row){
        const ScaledItem scaled_item = scaled_items[row];
        const detail::KnapsackItem& item = items[scaled_item.index];
        current_sum += scaled_item.value;
        for(std::size_t sum = current_sum + 1; sum-- > scaled_item.value;){
            const std::size_t previous = sum - scaled_item.value;
            if(reachable[previous] == 0) continue;
            if(item.weight > normalized_capacity - minimum_weight[previous]) continue;
            const std::uint64_t next_weight = minimum_weight[previous] + item.weight;
            if(reachable[sum] != 0 && minimum_weight[sum] <= next_weight) continue;
            minimum_weight[sum] = next_weight;
            reachable[sum] = 1;
            take[row][sum / 64U] |= std::uint64_t{1} << (sum % 64U);
        }
    }

    std::size_t best_sum = current_sum;
    while(reachable[best_sum] == 0) --best_sum;
    for(std::size_t row = scaled_items.size(); row-- > 0;){
        if(((take[row][best_sum / 64U] >> (best_sum % 64U)) & std::uint64_t{1}) == 0) continue;
        const ScaledItem scaled_item = scaled_items[row];
        selected[scaled_item.index] = true;
        best_sum -= scaled_item.value;
    }
    return detail::make_knapsack_result(items, std::move(selected));
}

}  // namespace approximate::dp
