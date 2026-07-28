#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_DIVIDE_AND_CONQUER_DP_OPTIMIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_DIVIDE_AND_CONQUER_DP_OPTIMIZATION_HPP_INCLUDED

#include <cstddef>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Value>
struct DivideAndConquerDPResult{
    std::vector<Value> value;
    std::vector<int> optimum;
};

template<class Value, class Cost>
DivideAndConquerDPResult<Value> divide_and_conquer_dp_layer(
    const std::vector<Value>& previous,
    Cost cost,
    Value infinity
){
    if(previous.empty())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: empty previous layer "
            "(divide_and_conquer_dp_layer)."
        );
    }
    const int size = static_cast<int>(previous.size()) - 1;
    DivideAndConquerDPResult<Value> result{
        std::vector<Value>(previous.size(), infinity),
        std::vector<int>(previous.size(), -1)
    };
    const auto solve = [&](auto&& self,
        int first, int last, int first_candidate, int last_candidate) -> void{
        if(first >= last) return;
        const int middle = first + (last - first) / 2;
        const int candidate_end = std::min(last_candidate, middle - 1);
        int best_candidate = -1;
        Value best = infinity;
        for(int candidate = first_candidate;
            candidate <= candidate_end;
            ++candidate){
            const Value value = previous[static_cast<std::size_t>(candidate)]
                + std::invoke(cost, candidate, middle);
            if(best_candidate == -1 || value < best){
                best = value;
                best_candidate = candidate;
            }
        }
        result.value[static_cast<std::size_t>(middle)] = best;
        result.optimum[static_cast<std::size_t>(middle)] = best_candidate;
        self(self, first, middle,
            first_candidate,
            best_candidate == -1 ? candidate_end : best_candidate);
        self(self, middle + 1, last,
            best_candidate == -1 ? first_candidate : best_candidate,
            last_candidate);
    };
    if(size != 0) solve(solve, 1, size + 1, 0, size - 1);
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_DIVIDE_AND_CONQUER_DP_OPTIMIZATION_HPP_INCLUDED
