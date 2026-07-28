#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_PROFILE_GRID_BROKEN_PROFILE_DP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_PROFILE_GRID_BROKEN_PROFILE_DP_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace dynamic_programming {

template<class Count, class Transition>
std::vector<Count> broken_profile_dp(
    std::size_t row_count,
    std::size_t column_count,
    std::size_t initial_mask,
    Transition transition
) {
    if(column_count >= std::numeric_limits<std::size_t>::digits){
        throw std::invalid_argument(
            "broken profile width does not fit size_t mask"
        );
    }
    const std::size_t state_count = std::size_t{1} << column_count;
    if(initial_mask >= state_count){
        throw std::invalid_argument("initial broken-profile mask is invalid");
    }
    std::vector<Count> current(state_count);
    std::vector<Count> next(state_count);
    current[initial_mask] = Count{1};
    for(std::size_t row = 0; row < row_count; ++row){
        for(std::size_t column = 0; column < column_count; ++column){
            std::fill(next.begin(), next.end(), Count{});
            for(std::size_t mask = 0; mask < state_count; ++mask){
                if(current[mask] == Count{}) continue;
                const auto emit = [&](std::size_t next_mask, const Count& multiplier){
                    if(next_mask >= state_count){
                        throw std::out_of_range(
                            "broken-profile transition emitted an invalid mask"
                        );
                    }
                    next[next_mask] += current[mask] * multiplier;
                };
                transition(row, column, mask, emit);
            }
            current.swap(next);
        }
    }
    return current;
}

template<class Count = std::uint64_t>
Count count_domino_tilings_broken_profile(
    std::size_t row_count,
    std::size_t column_count
) {
    if(column_count > row_count) std::swap(row_count, column_count);
    const auto result = broken_profile_dp<Count>(
        row_count,
        column_count,
        0,
        [row_count, column_count](
            std::size_t row,
            std::size_t column,
            std::size_t mask,
            const auto& emit
        ){
            if(mask & 1U){
                emit(mask >> 1, Count{1});
                return;
            }
            if(column + 1 < column_count && !(mask & 2U)){
                emit((mask | 2U) >> 1, Count{1});
            }
            if(row + 1 < row_count){
                emit(
                    (mask >> 1)
                        | (std::size_t{1} << (column_count - 1)),
                    Count{1}
                );
            }
        }
    );
    return result[0];
}

}  // namespace dynamic_programming

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_PROFILE_GRID_BROKEN_PROFILE_DP_HPP_INCLUDED
