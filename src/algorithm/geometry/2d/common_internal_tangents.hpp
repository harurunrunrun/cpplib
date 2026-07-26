#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_COMMON_INTERNAL_TANGENTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_COMMON_INTERNAL_TANGENTS_HPP_INCLUDED

#include <vector>

#include "common_tangents.hpp"

inline std::vector<Line> common_internal_tangents(
    const Circle& first,
    const Circle& second
){
    return common_tangents_detail::common_tangents_with_sides(
        first, second, {-1}
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_COMMON_INTERNAL_TANGENTS_HPP_INCLUDED
