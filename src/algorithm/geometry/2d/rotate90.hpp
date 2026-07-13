#pragma once

#include "types.hpp"

inline Point rotate90(const Point& point){
    return {-point.y, point.x};
}
