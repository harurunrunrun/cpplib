#pragma once

#include "types.hpp"

inline Point triangle_centroid(
    const Point& first,
    const Point& second,
    const Point& third
){
    return first +
        (second - first) / 3.0L +
        (third - first) / 3.0L;
}
