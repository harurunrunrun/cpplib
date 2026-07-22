#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_HISTOGRAM_PERIMETER_PERMUTATIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_HISTOGRAM_PERIMETER_PERMUTATIONS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <vector>

struct HistogramPerimeterPermutations {
    long long maximum_perimeter;
    unsigned long long permutation_count;
};

inline HistogramPerimeterPermutations maximum_histogram_perimeter_permutations(
    const std::vector<int>& heights
){
    const int n = static_cast<int>(heights.size());
    if(n == 0) return {0, 1};
    if(n >= 31)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: too many bars "
            "(maximum_histogram_perimeter_permutations)."
        );
    }
    for(int i = 0; i < n; i++){
        if(heights[static_cast<std::size_t>(i)] <= 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: nonpositive height "
                "(maximum_histogram_perimeter_permutations)."
            );
        }
        for(int j = 0; j < i; j++){
            if(heights[static_cast<std::size_t>(i)] == heights[static_cast<std::size_t>(j)])[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: duplicate height "
                    "(maximum_histogram_perimeter_permutations)."
                );
            }
        }
    }
    struct State {
        long long value = std::numeric_limits<long long>::min();
        unsigned long long count = 0;
    };
    const int mask_count = 1 << n;
    std::vector<State> dp(static_cast<std::size_t>(mask_count * n));
    auto state = [&](int mask, int last) -> State& {
        return dp[static_cast<std::size_t>(mask * n + last)];
    };
    for(int first = 0; first < n; first++){
        state(1 << first, first) = {heights[static_cast<std::size_t>(first)], 1};
    }
    for(int mask = 1; mask < mask_count; mask++){
        for(int last = 0; last < n; last++){
            const State current = state(mask, last);
            if(current.count == 0) continue;
            for(int next = 0; next < n; next++){
                if(mask & (1 << next)) continue;
                const long long candidate = current.value +
                    std::llabs(static_cast<long long>(heights[static_cast<std::size_t>(last)]) -
                               heights[static_cast<std::size_t>(next)]);
                State& destination = state(mask | (1 << next), next);
                if(candidate > destination.value){
                    destination = {candidate, current.count};
                }else if(candidate == destination.value){
                    destination.count += current.count;
                }
            }
        }
    }
    const int full = mask_count - 1;
    long long maximum = std::numeric_limits<long long>::min();
    unsigned long long count = 0;
    for(int last = 0; last < n; last++){
        const State current = state(full, last);
        const long long perimeter = current.value +
            heights[static_cast<std::size_t>(last)] + 2LL * n;
        if(perimeter > maximum){
            maximum = perimeter;
            count = current.count;
        }else if(perimeter == maximum){
            count += current.count;
        }
    }
    return {maximum, count};
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_HISTOGRAM_PERIMETER_PERMUTATIONS_HPP_INCLUDED
