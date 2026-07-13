#pragma once

#include <cmath>

#include "norm.hpp"

inline long double abs(const Point& point){
    return std::sqrt(norm(point));
}
