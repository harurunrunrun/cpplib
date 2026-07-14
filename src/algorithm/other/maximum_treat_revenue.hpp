#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

inline long long maximum_treat_revenue(const std::vector<int>& values){
    const int n = static_cast<int>(values.size());
    if(n == 0) return 0;
    for(int value: values){
        if(value < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative treat value "
                "(maximum_treat_revenue)."
            );
        }
    }
    std::vector<long long> dp(static_cast<std::size_t>(n));
    for(int left = 0; left < n; left++){
        dp[static_cast<std::size_t>(left)] =
            static_cast<long long>(n) * values[static_cast<std::size_t>(left)];
    }
    for(int length = 2; length <= n; length++){
        const int day = n - length + 1;
        for(int left = 0; left + length <= n; left++){
            const int right = left + length - 1;
            dp[static_cast<std::size_t>(left)] = std::max(
                static_cast<long long>(day) * values[static_cast<std::size_t>(left)] +
                    dp[static_cast<std::size_t>(left + 1)],
                static_cast<long long>(day) * values[static_cast<std::size_t>(right)] +
                    dp[static_cast<std::size_t>(left)]
            );
        }
    }
    return dp.front();
}
