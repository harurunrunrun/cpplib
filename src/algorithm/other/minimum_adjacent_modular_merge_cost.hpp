#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_adjacent_modular_merge_cost(
    const std::vector<long long>& values,
    long long modulus
){
    if(modulus <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: nonpositive modulus "
            "(minimum_adjacent_modular_merge_cost)."
        );
    }
    if(values.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: sequence is too large "
            "(minimum_adjacent_modular_merge_cost)."
        );
    }
    const int n = static_cast<int>(values.size());
    if(n == 0) return 0;
    std::vector<long long> prefix(static_cast<std::size_t>(n + 1));
    for(int i = 0; i < n; i++){
        const long long value = values[static_cast<std::size_t>(i)];
        if(value < 0 || value >= modulus)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: residue out of range "
                "(minimum_adjacent_modular_merge_cost)."
            );
        }
        prefix[static_cast<std::size_t>(i + 1)] =
            static_cast<long long>(
                (static_cast<__int128>(prefix[static_cast<std::size_t>(i)]) + value) %
                modulus
            );
    }
    auto residue = [&](int left, int right){
        const long long difference =
            prefix[static_cast<std::size_t>(right)] -
            prefix[static_cast<std::size_t>(left)];
        return difference < 0 ? difference + modulus : difference;
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
                const __int128 candidate =
                    static_cast<__int128>(
                        dp[static_cast<std::size_t>(left)][static_cast<std::size_t>(split - 1)]
                    ) +
                    dp[static_cast<std::size_t>(split)][static_cast<std::size_t>(right - 1)] +
                    static_cast<__int128>(residue(left, split)) * residue(split, right);
                if(candidate <= std::numeric_limits<long long>::max()){
                    best = std::min(best, static_cast<long long>(candidate));
                }
            }
            if(best == std::numeric_limits<long long>::max())[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: result overflow "
                    "(minimum_adjacent_modular_merge_cost)."
                );
            }
            dp[static_cast<std::size_t>(left)][static_cast<std::size_t>(right - 1)] = best;
        }
    }
    return dp.front().back();
}
