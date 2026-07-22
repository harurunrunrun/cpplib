#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMUM_ORDERED_PAIR_ASSIGNMENT_COST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMUM_ORDERED_PAIR_ASSIGNMENT_COST_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

struct OrderedPairAssignmentCosts {
    long long opening_cost;
    long long closing_cost;
};

inline long long minimum_ordered_pair_assignment_cost(
    const std::vector<OrderedPairAssignmentCosts>& ordered_costs
){
    if(ordered_costs.size() % 2 != 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: item count must be even "
            "(minimum_ordered_pair_assignment_cost)."
        );
    }
    if(ordered_costs.size() >
       static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error(
            "library assertion fault: item sequence is too long "
            "(minimum_ordered_pair_assignment_cost)."
        );
    }

    using Wide = __int128_t;
    std::priority_queue<Wide, std::vector<Wide>, std::greater<Wide>>
        opening_deltas;
    Wide result = 0;
    int prefix_balance = 0;

    for(const auto& costs: ordered_costs){
        result += static_cast<Wide>(costs.closing_cost);
        opening_deltas.push(
            static_cast<Wide>(costs.opening_cost) - costs.closing_cost
        );
        prefix_balance--;
        if(prefix_balance < 0){
            result += opening_deltas.top();
            opening_deltas.pop();
            prefix_balance += 2;
        }
    }

    if(result < std::numeric_limits<long long>::min() ||
       result > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "library assertion fault: result overflow "
            "(minimum_ordered_pair_assignment_cost)."
        );
    }
    return static_cast<long long>(result);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMUM_ORDERED_PAIR_ASSIGNMENT_COST_HPP_INCLUDED
