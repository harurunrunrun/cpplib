#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long count_perfect_assignments(
    const std::vector<std::vector<int>>& preferences
){
    const int n = static_cast<int>(preferences.size());
    if(n >= 31)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: too many students (count_perfect_assignments)."
        );
    }
    for(const auto& row: preferences){
        if(static_cast<int>(row.size()) != n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: non-square preferences "
                "(count_perfect_assignments)."
            );
        }
        for(int value: row){
            if(value != 0 && value != 1)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: nonbinary preference "
                    "(count_perfect_assignments)."
                );
            }
        }
    }
    const int mask_count = 1 << n;
    std::vector<long long> dp(static_cast<std::size_t>(mask_count));
    dp[0] = 1;
    for(int mask = 0; mask < mask_count; mask++){
        const int student = __builtin_popcount(static_cast<unsigned int>(mask));
        if(student == n) continue;
        for(int topic = 0; topic < n; topic++){
            if((mask & (1 << topic)) ||
               !preferences[static_cast<std::size_t>(student)][static_cast<std::size_t>(topic)]) continue;
            auto& destination = dp[static_cast<std::size_t>(mask | (1 << topic))];
            if(destination > std::numeric_limits<long long>::max() - dp[static_cast<std::size_t>(mask)])[[unlikely]]{
                throw std::overflow_error(
                    "library assertion fault: result overflow (count_perfect_assignments)."
                );
            }
            destination += dp[static_cast<std::size_t>(mask)];
        }
    }
    return dp.back();
}
