#ifndef CPPLIB_SRC_ALGORITHM_OTHER_MAXIMUM_TIME_WEIGHTED_ENDPOINT_REMOVAL_SCORE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_MAXIMUM_TIME_WEIGHTED_ENDPOINT_REMOVAL_SCORE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long maximum_time_weighted_endpoint_removal_score(
    const std::vector<long long>& values
){
    if(values.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: sequence is too large "
            "(maximum_time_weighted_endpoint_removal_score)."
        );
    }
    const int n = static_cast<int>(values.size());
    if(n == 0) return 0;
    auto checked_score = [](long long multiplier, long long value, long long suffix){
        const __int128 result =
            static_cast<__int128>(multiplier) * value + suffix;
        if(result < std::numeric_limits<long long>::min() ||
           result > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: score overflow "
                "(maximum_time_weighted_endpoint_removal_score)."
            );
        }
        return static_cast<long long>(result);
    };
    std::vector<long long> dp(static_cast<std::size_t>(n));
    for(int left = 0; left < n; left++){
        dp[static_cast<std::size_t>(left)] = checked_score(
            n, values[static_cast<std::size_t>(left)], 0
        );
    }
    for(int length = 2; length <= n; length++){
        const int day = n - length + 1;
        for(int left = 0; left + length <= n; left++){
            const int right = left + length - 1;
            dp[static_cast<std::size_t>(left)] = std::max(
                checked_score(
                    day, values[static_cast<std::size_t>(left)],
                    dp[static_cast<std::size_t>(left + 1)]
                ),
                checked_score(
                    day, values[static_cast<std::size_t>(right)],
                    dp[static_cast<std::size_t>(left)]
                )
            );
        }
    }
    return dp.front();
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_MAXIMUM_TIME_WEIGHTED_ENDPOINT_REMOVAL_SCORE_HPP_INCLUDED
