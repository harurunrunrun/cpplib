#pragma once

#include <cstddef>
#include <vector>

#include "bentley_ottmann_intersections.hpp"

inline std::size_t segment_intersection_count(
    const std::vector<Segment>& segments
){
    return bentley_ottmann_intersections(segments).size();
}
