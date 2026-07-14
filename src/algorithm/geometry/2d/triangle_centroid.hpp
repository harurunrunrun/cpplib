#pragma once

#include "types.hpp"

inline Point triangle_centroid(
    const Point& first,
    const Point& second,
    const Point& third
){
    return (first + second + third) / 3.0L;
}
