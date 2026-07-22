#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_WEIGHT_NONOVERLAPPING_INTERVALS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_WEIGHT_NONOVERLAPPING_INTERVALS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

struct WeightedInterval {
    long long begin;
    long long end;
    long long weight;
};

inline long long maximum_weight_nonoverlapping_intervals(
    std::vector<WeightedInterval> intervals
){
    for(const auto& interval: intervals){
        if(interval.begin >= interval.end || interval.weight < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid weighted interval "
                "(maximum_weight_nonoverlapping_intervals)."
            );
        }
    }
    std::sort(intervals.begin(), intervals.end(), [](const auto& left, const auto& right){
        if(left.end != right.end) return left.end < right.end;
        return left.begin < right.begin;
    });
    const int n = static_cast<int>(intervals.size());
    std::vector<long long> ends(static_cast<std::size_t>(n));
    std::vector<long long> dp(static_cast<std::size_t>(n + 1));
    for(int i = 0; i < n; i++){
        const auto& interval = intervals[static_cast<std::size_t>(i)];
        ends[static_cast<std::size_t>(i)] = interval.end;
        const int compatible = static_cast<int>(
            std::upper_bound(
                ends.begin(), ends.begin() + i,
                interval.begin
            ) - ends.begin()
        );
        const __int128 take =
            static_cast<__int128>(dp[static_cast<std::size_t>(compatible)]) +
            static_cast<__int128>(interval.weight);
        if(take > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: result overflow "
                "(maximum_weight_nonoverlapping_intervals)."
            );
        }
        dp[static_cast<std::size_t>(i + 1)] = std::max(
            dp[static_cast<std::size_t>(i)],
            static_cast<long long>(take)
        );
    }
    return dp.back();
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_WEIGHT_NONOVERLAPPING_INTERVALS_HPP_INCLUDED
