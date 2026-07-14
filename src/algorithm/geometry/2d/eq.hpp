#pragma once

#include "geometry_sign.hpp"

inline bool eq(long double first, long double second){
    return geometry_sign(first - second) == 0;
}
