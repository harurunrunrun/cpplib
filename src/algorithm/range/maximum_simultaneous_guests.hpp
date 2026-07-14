#pragma once

#include <utility>
#include <vector>

#include "maximum_interval_overlap.hpp"

inline int maximum_simultaneous_guests(
    const std::vector<std::pair<int, int>>& stays
){
    return maximum_interval_overlap(stays, IntervalEndpointPolicy::half_open);
}
