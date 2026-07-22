#ifndef CPPLIB_SRC_ALGORITHM_OTHER_COUNT_THREE_ROW_DOMINO_TILINGS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_COUNT_THREE_ROW_DOMINO_TILINGS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace count_three_row_domino_tilings_internal {
inline std::uint64_t fixed_height(int height, int width){
    if(width < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative width (domino tilings)."
        );
    }
    const int state_count = 1 << height;
    std::vector<std::uint64_t> dp(static_cast<std::size_t>(state_count));
    std::vector<std::uint64_t> next(static_cast<std::size_t>(state_count));
    dp[0] = 1;
    for(int column = 0; column < width; column++){
        std::fill(next.begin(), next.end(), 0);
        for(int mask = 0; mask < state_count; mask++){
            const auto ways = dp[static_cast<std::size_t>(mask)];
            if(ways == 0) continue;
            auto fill = [&](auto&& self, int row, int occupied, int outgoing) -> void {
                while(row < height && (occupied & (1 << row))) row++;
                if(row == height){
                    auto& destination = next[static_cast<std::size_t>(outgoing)];
                    if(destination > std::numeric_limits<std::uint64_t>::max() - ways)[[unlikely]]{
                        throw std::overflow_error(
                            "library assertion fault: result overflow (domino tilings)."
                        );
                    }
                    destination += ways;
                    return;
                }
                if(row + 1 < height && !(occupied & (1 << (row + 1)))){
                    self(self, row + 2, occupied | (1 << row) | (1 << (row + 1)), outgoing);
                }
                self(self, row + 1, occupied | (1 << row), outgoing | (1 << row));
            };
            fill(fill, 0, mask, 0);
        }
        dp.swap(next);
    }
    return dp[0];
}
} // namespace count_three_row_domino_tilings_internal

inline std::uint64_t count_three_row_domino_tilings(int width){
    return count_three_row_domino_tilings_internal::fixed_height(3, width);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COUNT_THREE_ROW_DOMINO_TILINGS_HPP_INCLUDED
