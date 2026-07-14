#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

inline unsigned long long count_nondecreasing_digit_sum_partitions(
    const std::string& digits
){
    const int n = static_cast<int>(digits.size());
    if(n == 0) return 1;
    for(char digit: digits){
        if(digit < '0' || digit > '9')[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: nondigit character "
                "(count_nondecreasing_digit_sum_partitions)."
            );
        }
    }
    const int maximum_sum = 9 * n;
    std::vector<std::vector<unsigned long long>> dp(
        static_cast<std::size_t>(n + 1),
        std::vector<unsigned long long>(static_cast<std::size_t>(maximum_sum + 1))
    );
    for(int minimum = 0; minimum <= maximum_sum; minimum++){
        dp[static_cast<std::size_t>(n)][static_cast<std::size_t>(minimum)] = 1;
    }
    for(int position = n - 1; position >= 0; position--){
        for(int minimum = 0; minimum <= maximum_sum; minimum++){
            int sum = 0;
            unsigned long long ways = 0;
            for(int end = position; end < n; end++){
                sum += digits[static_cast<std::size_t>(end)] - '0';
                if(sum < minimum) continue;
                const auto add = dp[static_cast<std::size_t>(end + 1)][static_cast<std::size_t>(sum)];
                if(ways > std::numeric_limits<unsigned long long>::max() - add)[[unlikely]]{
                    throw std::overflow_error(
                        "library assertion fault: result overflow "
                        "(count_nondecreasing_digit_sum_partitions)."
                    );
                }
                ways += add;
            }
            dp[static_cast<std::size_t>(position)][static_cast<std::size_t>(minimum)] = ways;
        }
    }
    return dp.front().front();
}
