#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_mixture_smoke(const std::vector<int>& colors){
    const int n = static_cast<int>(colors.size());
    if(n == 0) return 0;
    std::vector<int> prefix(static_cast<std::size_t>(n + 1));
    for(int i = 0; i < n; i++){
        if(colors[static_cast<std::size_t>(i)] < 0 ||
           colors[static_cast<std::size_t>(i)] >= 100)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: color out of range "
                "(minimum_mixture_smoke)."
            );
        }
        prefix[static_cast<std::size_t>(i + 1)] =
            (prefix[static_cast<std::size_t>(i)] +
             colors[static_cast<std::size_t>(i)]) % 100;
    }
    auto color = [&](int left, int right){
        return (prefix[static_cast<std::size_t>(right)] -
                prefix[static_cast<std::size_t>(left)] + 100) % 100;
    };
    std::vector<std::vector<long long>> dp(
        static_cast<std::size_t>(n),
        std::vector<long long>(static_cast<std::size_t>(n))
    );
    for(int span = 2; span <= n; span++){
        for(int left = 0; left + span <= n; left++){
            const int right = left + span;
            long long best = std::numeric_limits<long long>::max();
            for(int split = left + 1; split < right; split++){
                best = std::min(
                    best,
                    dp[static_cast<std::size_t>(left)][static_cast<std::size_t>(split - 1)] +
                    dp[static_cast<std::size_t>(split)][static_cast<std::size_t>(right - 1)] +
                    static_cast<long long>(color(left, split)) * color(split, right)
                );
            }
            dp[static_cast<std::size_t>(left)][static_cast<std::size_t>(right - 1)] = best;
        }
    }
    return dp.front().back();
}
