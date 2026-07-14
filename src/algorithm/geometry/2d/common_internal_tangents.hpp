#pragma once

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
