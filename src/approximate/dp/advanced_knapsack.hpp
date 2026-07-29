#ifndef CPPLIB_SRC_APPROXIMATE_DP_ADVANCED_KNAPSACK_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_DP_ADVANCED_KNAPSACK_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "knapsack.hpp"

namespace approximate::dp {

/**
 * Solves a small ratio-ordered core exactly and keeps the usual modified-greedy
 * solution as a fallback.
 *
 * `core_size` must not exceed 24.  Complexity is
 * O(n log n + core_size * 2^core_size) time and O(n) space.
 */
template<class Weight, class Value, class Capacity>
[[nodiscard]] KnapsackResult core_problem_knapsack(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity,
    std::size_t core_size = 18
){
    if(core_size > 24)[[unlikely]]{
        throw std::invalid_argument("core_size must not exceed 24");
    }
    auto [items, normalized_capacity] =
        detail::normalize_knapsack(weights, values, capacity);
    KnapsackResult best =
        modified_greedy_knapsack(weights, values, capacity);
    if(core_size == 0 || items.empty()) return best;

    std::vector<std::size_t> order;
    order.reserve(items.size());
    std::vector<bool> fixed(items.size(), false);
    for(const detail::KnapsackItem& item : items){
        if(item.weight == 0 && item.value != 0){
            fixed[item.index] = true;
        }else if(item.weight <= normalized_capacity && item.value != 0){
            order.push_back(item.index);
        }
    }
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        return detail::RatioLess{}(items[left], items[right]);
    });
    if(order.empty()) return best;

    std::uint64_t greedy_weight = 0;
    std::size_t break_position = order.size();
    for(std::size_t position = 0; position < order.size(); ++position){
        const auto& item = items[order[position]];
        if(item.weight <= normalized_capacity - greedy_weight){
            greedy_weight += item.weight;
        }else{
            break_position = position;
            break;
        }
    }
    const std::size_t half = core_size / 2;
    const std::size_t begin = break_position > half ? break_position - half : 0;
    const std::size_t end = std::min(order.size(), begin + core_size);

    std::uint64_t fixed_weight = 0;
    for(std::size_t position = 0; position < begin; ++position){
        const std::size_t index = order[position];
        if(items[index].weight <= normalized_capacity - fixed_weight){
            fixed[index] = true;
            fixed_weight += items[index].weight;
        }
    }
    std::uint64_t fixed_value = 0;
    for(const detail::KnapsackItem& item : items){
        if(fixed[item.index]){
            fixed_value = detail::checked_add(
                fixed_value,
                item.value,
                "the core value does not fit in uint64_t"
            );
        }
    }
    const std::size_t count = end - begin;
    const std::uint64_t combinations = std::uint64_t{1} << count;
    std::uint64_t best_mask = 0;
    std::uint64_t best_core_value = fixed_value;
    for(std::uint64_t mask = 0; mask < combinations; ++mask){
        std::uint64_t used = fixed_weight;
        std::uint64_t candidate_value = fixed_value;
        bool feasible = true;
        for(std::size_t bit = 0; bit < count; ++bit){
            if(((mask >> bit) & std::uint64_t{1}) == 0) continue;
            const std::size_t index = order[begin + bit];
            if(items[index].weight > normalized_capacity - used){
                feasible = false;
                break;
            }
            used += items[index].weight;
            candidate_value = detail::checked_add(
                candidate_value,
                items[index].value,
                "the core value does not fit in uint64_t"
            );
        }
        if(feasible && candidate_value > best_core_value){
            best_core_value = candidate_value;
            best_mask = mask;
        }
    }
    if(best_core_value > best.total_value){
        for(std::size_t bit = 0; bit < count; ++bit){
            if(((best_mask >> bit) & std::uint64_t{1}) != 0){
                fixed[order[begin + bit]] = true;
            }
        }
        best = detail::make_knapsack_result(items, std::move(fixed));
    }
    return best;
}

/**
 * Capacity-indexed DP after rounding each positive weight upward.
 *
 * The upward rounding keeps every returned solution feasible.  The
 * `maximum_scaled_capacity` parameter bounds the state array.
 *
 * Complexity: O(n * maximum_scaled_capacity) time and
 * O(n * maximum_scaled_capacity / 64 + maximum_scaled_capacity) space.
 */
template<class Weight, class Value, class Capacity>
[[nodiscard]] KnapsackResult weight_scaling_knapsack_dp(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity,
    std::size_t maximum_scaled_capacity
){
    if(maximum_scaled_capacity == 0)[[unlikely]]{
        throw std::invalid_argument("maximum_scaled_capacity must be positive");
    }
    auto [items, normalized_capacity] =
        detail::normalize_knapsack(weights, values, capacity);
    std::vector<bool> selected(items.size(), false);
    for(const detail::KnapsackItem& item : items){
        if(item.weight == 0 && item.value != 0) selected[item.index] = true;
    }
    if(normalized_capacity == 0){
        return detail::make_knapsack_result(items, std::move(selected));
    }
    const std::uint64_t divisor =
        normalized_capacity / maximum_scaled_capacity
        + static_cast<std::uint64_t>(
            normalized_capacity % maximum_scaled_capacity != 0
        );
    const std::size_t scaled_capacity = static_cast<std::size_t>(
        normalized_capacity / divisor
    );
    if(scaled_capacity == 0){
        return detail::make_knapsack_result(items, std::move(selected));
    }

    struct ScaledItem {
        std::size_t index = 0;
        std::size_t weight = 0;
    };
    std::vector<ScaledItem> scaled;
    for(const detail::KnapsackItem& item : items){
        if(item.weight == 0 || item.value == 0) continue;
        const std::uint64_t rounded =
            item.weight / divisor
            + static_cast<std::uint64_t>(item.weight % divisor != 0);
        if(rounded <= scaled_capacity){
            scaled.push_back({item.index, static_cast<std::size_t>(rounded)});
        }
    }
    if(scaled_capacity
       == std::numeric_limits<std::size_t>::max())[[unlikely]]{
        throw std::length_error(
            "the scaled capacity does not fit in a state array"
        );
    }
    const std::size_t state_count = scaled_capacity + 1;
    const std::size_t words = state_count / 64U
        + static_cast<std::size_t>(state_count % 64U != 0);
    if(words != 0
       && scaled.size() > std::numeric_limits<std::size_t>::max() / words){
        throw std::length_error("weight-scaling reconstruction table is too large");
    }
    std::vector<std::uint64_t> best_value(state_count, 0);
    std::vector<unsigned char> reachable(state_count, 0);
    reachable[0] = 1;
    std::vector<std::vector<std::uint64_t>> take(
        scaled.size(), std::vector<std::uint64_t>(words, 0)
    );
    for(std::size_t row = 0; row < scaled.size(); ++row){
        const auto scaled_item = scaled[row];
        const auto& item = items[scaled_item.index];
        for(std::size_t used = state_count; used-- > scaled_item.weight;){
            const std::size_t previous = used - scaled_item.weight;
            if(reachable[previous] == 0) continue;
            const std::uint64_t candidate = detail::checked_add(
                best_value[previous],
                item.value,
                "the DP value does not fit in uint64_t"
            );
            if(reachable[used] != 0 && best_value[used] >= candidate) continue;
            reachable[used] = 1;
            best_value[used] = candidate;
            take[row][used / 64U] |= std::uint64_t{1} << (used % 64U);
        }
    }
    std::size_t best_state = 0;
    for(std::size_t state = 1; state < state_count; ++state){
        if(reachable[state] != 0 && best_value[state] > best_value[best_state]){
            best_state = state;
        }
    }
    for(std::size_t row = scaled.size(); row-- > 0;){
        if(((take[row][best_state / 64U] >> (best_state % 64U))
            & std::uint64_t{1}) == 0){
            continue;
        }
        selected[scaled[row].index] = true;
        best_state -= scaled[row].weight;
    }
    return detail::make_knapsack_result(items, std::move(selected));
}

struct MultiKnapsackResult {
    std::vector<std::size_t> selected_indices;
    std::vector<std::uint64_t> total_weights;
    std::uint64_t total_value = 0;
};

/**
 * Greedy multi-constraint knapsack after combining constraints by nonnegative
 * surrogate multipliers.
 *
 * Complexity: O(mn + n log n) time and O(mn) space.
 */
template<class Weight, class Value, class Capacity, class Real>
[[nodiscard]] MultiKnapsackResult surrogate_relaxation_knapsack(
    const std::vector<std::vector<Weight>>& constraint_weights,
    const std::vector<Value>& values,
    const std::vector<Capacity>& capacities,
    const std::vector<Real>& multipliers
){
    static_assert(std::is_integral_v<Weight>
                  && !std::is_same_v<Weight, bool>);
    static_assert(std::is_integral_v<Value>
                  && !std::is_same_v<Value, bool>);
    static_assert(std::is_integral_v<Capacity>
                  && !std::is_same_v<Capacity, bool>);
    static_assert(std::is_floating_point_v<Real>);
    const std::size_t constraints = constraint_weights.size();
    if(constraints == 0 || capacities.size() != constraints
       || multipliers.size() != constraints)[[unlikely]]{
        throw std::invalid_argument("surrogate constraint dimensions differ");
    }
    std::vector<std::uint64_t> normalized_capacities(constraints);
    bool positive_multiplier = false;
    for(std::size_t constraint = 0; constraint < constraints; ++constraint){
        if(constraint_weights[constraint].size() != values.size())[[unlikely]]{
            throw std::invalid_argument("surrogate weight row has wrong size");
        }
        normalized_capacities[constraint] = detail::nonnegative_integer(
            capacities[constraint], "capacities must be nonnegative"
        );
        if(!std::isfinite(multipliers[constraint])
           || multipliers[constraint] < Real{})[[unlikely]]{
            throw std::invalid_argument(
                "surrogate multipliers must be finite and nonnegative"
            );
        }
        positive_multiplier =
            positive_multiplier || multipliers[constraint] > Real{};
    }
    if(!positive_multiplier)[[unlikely]]{
        throw std::invalid_argument("at least one surrogate multiplier is positive");
    }

    std::vector<std::uint64_t> normalized_values(values.size());
    std::vector<std::vector<std::uint64_t>> normalized_weights(
        constraints, std::vector<std::uint64_t>(values.size())
    );
    std::vector<long double> surrogate(values.size(), 0.0L);
    for(std::size_t item = 0; item < values.size(); ++item){
        normalized_values[item] = detail::nonnegative_integer(
            values[item], "values must be nonnegative"
        );
        for(std::size_t constraint = 0; constraint < constraints; ++constraint){
            normalized_weights[constraint][item] = detail::nonnegative_integer(
                constraint_weights[constraint][item],
                "weights must be nonnegative"
            );
            surrogate[item] += static_cast<long double>(
                multipliers[constraint]
            ) * static_cast<long double>(normalized_weights[constraint][item]);
        }
        if(!std::isfinite(surrogate[item]))[[unlikely]]{
            throw std::overflow_error(
                "a surrogate weight is not finite"
            );
        }
    }
    std::vector<std::size_t> order(values.size());
    for(std::size_t item = 0; item < values.size(); ++item) order[item] = item;
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        if(surrogate[left] == 0.0L || surrogate[right] == 0.0L){
            if(surrogate[left] == 0.0L && surrogate[right] != 0.0L) return true;
            if(surrogate[left] != 0.0L && surrogate[right] == 0.0L) return false;
        }
        const long double lhs =
            static_cast<long double>(normalized_values[left]) * surrogate[right];
        const long double rhs =
            static_cast<long double>(normalized_values[right]) * surrogate[left];
        if(lhs != rhs) return lhs > rhs;
        return left < right;
    });

    MultiKnapsackResult result;
    result.total_weights.assign(constraints, 0);
    for(const std::size_t item : order){
        if(normalized_values[item] == 0) continue;
        bool feasible = true;
        for(std::size_t constraint = 0; constraint < constraints; ++constraint){
            feasible = feasible
                && normalized_weights[constraint][item]
                    <= normalized_capacities[constraint]
                        - result.total_weights[constraint];
        }
        if(!feasible) continue;
        result.selected_indices.push_back(item);
        for(std::size_t constraint = 0; constraint < constraints; ++constraint){
            result.total_weights[constraint] +=
                normalized_weights[constraint][item];
        }
        result.total_value = detail::checked_add(
            result.total_value,
            normalized_values[item],
            "the selected total value does not fit in uint64_t"
        );
    }
    std::sort(result.selected_indices.begin(), result.selected_indices.end());
    return result;
}

/**
 * Greedy feasible primal solution ordered by positive Lagrangian reduced profit.
 *
 * Complexity: O(n log n) time and O(n) space.
 */
template<class Weight, class Value, class Capacity, class Real>
[[nodiscard]] KnapsackResult lagrangian_relaxation_knapsack(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity,
    Real penalty
){
    static_assert(std::is_floating_point_v<Real>);
    if(!std::isfinite(penalty) || penalty < Real{})[[unlikely]]{
        throw std::invalid_argument("Lagrangian penalty must be finite and nonnegative");
    }
    auto [items, normalized_capacity] =
        detail::normalize_knapsack(weights, values, capacity);
    std::vector<long double> reduced(items.size());
    std::vector<std::size_t> order;
    for(const detail::KnapsackItem& item : items){
        reduced[item.index] = static_cast<long double>(item.value)
            - static_cast<long double>(penalty)
                * static_cast<long double>(item.weight);
        if(item.value != 0 && reduced[item.index] > 0.0L){
            order.push_back(item.index);
        }
    }
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        if(reduced[left] != reduced[right]) return reduced[left] > reduced[right];
        return left < right;
    });
    std::vector<bool> selected(items.size(), false);
    std::uint64_t used = 0;
    for(const std::size_t index : order){
        if(items[index].weight > normalized_capacity - used) continue;
        selected[index] = true;
        used += items[index].weight;
    }
    return detail::make_knapsack_result(items, std::move(selected));
}

struct DantzigBoundResult {
    KnapsackResult solution;
    long double fractional_upper_bound = 0.0L;
};

/**
 * Computes Dantzig's fractional-knapsack upper bound and a modified-greedy
 * integral solution.
 *
 * Complexity: O(n log n) time and O(n) space.
 */
template<class Weight, class Value, class Capacity>
[[nodiscard]] DantzigBoundResult dantzig_bound_knapsack(
    const std::vector<Weight>& weights,
    const std::vector<Value>& values,
    Capacity capacity
){
    auto [items, normalized_capacity] =
        detail::normalize_knapsack(weights, values, capacity);
    DantzigBoundResult result;
    result.solution = modified_greedy_knapsack(weights, values, capacity);
    std::vector<std::size_t> order;
    for(const detail::KnapsackItem& item : items){
        if(item.value == 0) continue;
        if(item.weight == 0){
            result.fractional_upper_bound +=
                static_cast<long double>(item.value);
        }else{
            order.push_back(item.index);
        }
    }
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        return detail::RatioLess{}(items[left], items[right]);
    });
    std::uint64_t remaining = normalized_capacity;
    for(const std::size_t index : order){
        const auto& item = items[index];
        if(item.weight <= remaining){
            remaining -= item.weight;
            result.fractional_upper_bound +=
                static_cast<long double>(item.value);
        }else{
            result.fractional_upper_bound +=
                static_cast<long double>(item.value)
                * static_cast<long double>(remaining)
                / static_cast<long double>(item.weight);
            break;
        }
    }
    return result;
}

}  // namespace approximate::dp

#endif  // CPPLIB_SRC_APPROXIMATE_DP_ADVANCED_KNAPSACK_HPP_INCLUDED
