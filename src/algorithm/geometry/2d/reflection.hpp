#pragma once

#include "projection.hpp"

inline Point reflection(const Line& line, const Point& point){
    return point + (projection(line, point) - point) * 2;
}
