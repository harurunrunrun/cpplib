#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMUM_EXACT_WEIGHT_COST_WITH_ITEM_LIMIT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMUM_EXACT_WEIGHT_COST_WITH_ITEM_LIMIT_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_exact_weight_cost_with_item_limit(
    int target_weight,
    int maximum_item_count,
    const std::vector<long long>& cost_by_weight
){
    if(target_weight < 0 || maximum_item_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative target or item limit "
            "(minimum_exact_weight_cost_with_item_limit)."
        );
    }
    if(cost_by_weight.size() >
       static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: cost table is too large "
            "(minimum_exact_weight_cost_with_item_limit)."
        );
    }
    for(long long cost: cost_by_weight){
        if(cost < -1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid item cost "
                "(minimum_exact_weight_cost_with_item_limit)."
            );
        }
    }
    constexpr __int128 infinity = static_cast<__int128>(1) << 126;
    const std::size_t state_count = static_cast<std::size_t>(target_weight) + 1;
    std::vector<__int128> previous(
        state_count, infinity
    );
    std::vector<__int128> current(
        state_count, infinity
    );
    previous[0] = 0;
    __int128 answer = target_weight == 0 ? 0 : infinity;
    const int item_limit = std::min(maximum_item_count, target_weight);
    const int maximum_weight = static_cast<int>(cost_by_weight.size());
    for(int count = 1; count <= item_limit; count++){
        std::fill(current.begin(), current.end(), infinity);
        for(int total = 1; total <= target_weight; total++){
            for(int weight = 1; weight <= std::min(total, maximum_weight); weight++){
                const long long cost =
                    cost_by_weight[static_cast<std::size_t>(weight - 1)];
                if(cost < 0 ||
                   previous[static_cast<std::size_t>(total - weight)] == infinity){
                    continue;
                }
                current[static_cast<std::size_t>(total)] = std::min(
                    current[static_cast<std::size_t>(total)],
                    previous[static_cast<std::size_t>(total - weight)] + cost
                );
            }
        }
        answer = std::min(answer, current[static_cast<std::size_t>(target_weight)]);
        previous.swap(current);
    }
    if(answer == infinity) return -1;
    if(answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: result overflow "
            "(minimum_exact_weight_cost_with_item_limit)."
        );
    }
    return static_cast<long long>(answer);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMUM_EXACT_WEIGHT_COST_WITH_ITEM_LIMIT_HPP_INCLUDED
