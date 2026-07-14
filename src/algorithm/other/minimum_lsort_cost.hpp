#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_lsort_cost(const std::vector<int>& permutation){
    const int n = static_cast<int>(permutation.size());
    if(n == 0) return 0;
    std::vector<int> position(static_cast<std::size_t>(n + 1), -1);
    for(int index = 0; index < n; index++){
        const int value = permutation[static_cast<std::size_t>(index)];
        if(value < 1 || value > n || position[static_cast<std::size_t>(value)] != -1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: input is not a permutation "
                "(minimum_lsort_cost)."
            );
        }
        position[static_cast<std::size_t>(value)] = index;
    }
    std::vector<std::vector<int>> before_prefix(
        static_cast<std::size_t>(n + 1),
        std::vector<int>(static_cast<std::size_t>(n + 1))
    );
    for(int selected = 1; selected <= n; selected++){
        for(int value = 1; value <= n; value++){
            before_prefix[static_cast<std::size_t>(selected)][static_cast<std::size_t>(value)] =
                before_prefix[static_cast<std::size_t>(selected)][static_cast<std::size_t>(value - 1)] +
                (position[static_cast<std::size_t>(value)] < position[static_cast<std::size_t>(selected)] ? 1 : 0);
        }
    }
    constexpr long long infinity = std::numeric_limits<long long>::max() / 4;
    std::vector<std::vector<long long>> dp(
        static_cast<std::size_t>(n + 2),
        std::vector<long long>(static_cast<std::size_t>(n + 2), infinity)
    );
    for(int value = 1; value <= n; value++){
        dp[static_cast<std::size_t>(value)][static_cast<std::size_t>(value)] =
            position[static_cast<std::size_t>(value)] + 1LL;
    }
    for(int length = 2; length <= n; length++){
        for(int left = 1; left + length - 1 <= n; left++){
            const int right = left + length - 1;
            const int removed_before_left =
                before_prefix[static_cast<std::size_t>(left)][static_cast<std::size_t>(right)] -
                before_prefix[static_cast<std::size_t>(left)][static_cast<std::size_t>(left)];
            const int rank_left = position[static_cast<std::size_t>(left)] + 1 - removed_before_left;
            const int removed_before_right =
                before_prefix[static_cast<std::size_t>(right)][static_cast<std::size_t>(right - 1)] -
                before_prefix[static_cast<std::size_t>(right)][static_cast<std::size_t>(left - 1)];
            const int rank_right = position[static_cast<std::size_t>(right)] + 1 - removed_before_right;
            dp[static_cast<std::size_t>(left)][static_cast<std::size_t>(right)] = std::min(
                dp[static_cast<std::size_t>(left + 1)][static_cast<std::size_t>(right)] +
                    static_cast<long long>(rank_left) * length,
                dp[static_cast<std::size_t>(left)][static_cast<std::size_t>(right - 1)] +
                    static_cast<long long>(rank_right) * length
            );
        }
    }
    return dp[1][static_cast<std::size_t>(n)];
}
