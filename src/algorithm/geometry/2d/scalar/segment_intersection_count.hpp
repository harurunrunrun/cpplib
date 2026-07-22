#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SEGMENT_INTERSECTION_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SEGMENT_INTERSECTION_COUNT_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "../shape/bentley_ottmann_intersections.hpp"

inline std::size_t segment_intersection_count(
    const std::vector<Segment>& segments
){
    return bentley_ottmann_intersections(segments).size();
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SEGMENT_INTERSECTION_COUNT_HPP_INCLUDED
