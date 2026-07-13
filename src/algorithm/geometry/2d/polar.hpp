#pragma once

#include <cmath>

#include "types.hpp"

inline Point polar(long double radius, long double theta){
    return {radius * std::cos(theta), radius * std::sin(theta)};
}
