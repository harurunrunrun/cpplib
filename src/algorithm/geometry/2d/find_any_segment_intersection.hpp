#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_FIND_ANY_SEGMENT_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_FIND_ANY_SEGMENT_INTERSECTION_HPP_INCLUDED

#include <optional>
#include <utility>
#include <vector>

#include "bentley_ottmann_intersections.hpp"

inline std::optional<std::pair<std::size_t, std::size_t>>
find_any_segment_intersection(const std::vector<Segment>& segments){
    const auto intersections = bentley_ottmann_detail::intersections(
        segments, 1
    );
    if(intersections.empty()) return std::nullopt;
    return std::pair{
        intersections.front().first, intersections.front().second
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_FIND_ANY_SEGMENT_INTERSECTION_HPP_INCLUDED
