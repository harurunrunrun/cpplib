#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_LAGRANGIAN_RELAXATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_LAGRANGIAN_RELAXATION_HPP_INCLUDED

#include <functional>
#include <stdexcept>
#include <utility>
#include <type_traits>

template<class Cost>
struct LagrangianRelaxationState{
    Cost penalized_cost;
    long long count;
};

template<class Cost, class Solver>
Cost lagrangian_relaxation_exact_count(
    long long target_count,
    Cost minimum_penalty,
    Cost maximum_penalty,
    Solver solve
){
    static_assert(std::is_integral_v<Cost>);
    if(target_count < 0 || maximum_penalty < minimum_penalty)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid range "
            "(lagrangian_relaxation_exact_count)."
        );
    }
    Cost left = minimum_penalty;
    Cost right = maximum_penalty;
    while(left < right){
        const Cost middle = left + (right - left) / Cost(2);
        const auto state = std::invoke(solve, middle);
        if(state.count <= target_count){
            right = middle;
        }else{
            left = middle + Cost(1);
        }
    }
    const auto state = std::invoke(solve, left);
    return state.penalized_cost - left * static_cast<Cost>(target_count);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_LAGRANGIAN_RELAXATION_HPP_INCLUDED
