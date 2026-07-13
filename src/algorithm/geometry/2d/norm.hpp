#pragma once

#include "dot.hpp"

inline long double norm(const Point& point){
    return dot(point, point);
}
