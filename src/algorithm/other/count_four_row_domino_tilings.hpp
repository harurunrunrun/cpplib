#pragma once

#include "count_three_row_domino_tilings.hpp"

inline std::uint64_t count_four_row_domino_tilings(int width){
    return count_three_row_domino_tilings_internal::fixed_height(4, width);
}
