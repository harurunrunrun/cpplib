#pragma once

#include "constants.hpp"

inline int geometry_sign(long double value){
    if(value > GEOMETRY_EPS) return 1;
    if(value < -GEOMETRY_EPS) return -1;
    return 0;
}
