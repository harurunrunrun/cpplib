#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_NONDECREASING_DIGIT_SUM_PARTITIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_NONDECREASING_DIGIT_SUM_PARTITIONS_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

inline unsigned long long count_nondecreasing_digit_sum_partitions(
    const std::string& digits
){
    if(digits.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max() / 9
    ))[[unlikely]]{
        throw std::length_error(
            "count_nondecreasing_digit_sum_partitions input is too long"
        );
    }
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
    using wide_type = __uint128_t;
    constexpr wide_type overflow =
        static_cast<wide_type>(std::numeric_limits<unsigned long long>::max()) + 1;
    const auto saturated_add = [overflow](wide_type lhs, wide_type rhs){
        return lhs >= overflow - rhs ? overflow : lhs + rhs;
    };
    std::vector<std::vector<wide_type>> dp(
        static_cast<std::size_t>(n + 1),
        std::vector<wide_type>(static_cast<std::size_t>(maximum_sum + 1))
    );
    dp[0][0] = 1;
    for(int position = 0; position < n; ++position){
        auto& prefix = dp[static_cast<std::size_t>(position)];
        for(int sum = 1; sum <= maximum_sum; ++sum){
            prefix[static_cast<std::size_t>(sum)] = saturated_add(
                prefix[static_cast<std::size_t>(sum)],
                prefix[static_cast<std::size_t>(sum - 1)]
            );
        }
        int sum = 0;
        for(int end = position; end < n; ++end){
            sum += digits[static_cast<std::size_t>(end)] - '0';
            auto& destination = dp[static_cast<std::size_t>(end + 1)]
                                  [static_cast<std::size_t>(sum)];
            destination = saturated_add(
                destination,
                prefix[static_cast<std::size_t>(sum)]
            );
        }
    }
    wide_type answer = 0;
    for(const auto ways: dp.back()) answer = saturated_add(answer, ways);
    if(answer == overflow)[[unlikely]]{
        throw std::overflow_error(
            "library assertion fault: result overflow "
            "(count_nondecreasing_digit_sum_partitions)."
        );
    }
    return static_cast<unsigned long long>(answer);
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_NONDECREASING_DIGIT_SUM_PARTITIONS_HPP_INCLUDED
