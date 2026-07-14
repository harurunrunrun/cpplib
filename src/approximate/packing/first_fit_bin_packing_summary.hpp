#pragma once

#include <cstddef>
#include <numeric>
#include <vector>

#include "bin_packing.hpp"

namespace approximate::packing{

struct FirstFitBinPackingSummary{
    std::size_t used_bin_count;
    long long unused_capacity;
};

inline FirstFitBinPackingSummary first_fit_bin_packing_summary(
    const std::vector<int>& item_size,
    int capacity
){
    const auto result = first_fit_bin_packing(item_size, capacity);
    return {
        result.bin_count(),
        std::accumulate(
            result.remaining_capacity.begin(),
            result.remaining_capacity.end(),
            0LL
        )
    };
}

} // namespace approximate::packing
