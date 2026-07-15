#pragma once

#include "base.hpp"
#include "circumcircle.hpp"

inline long double triangle_circumradius(const Triangle3& triangle){
    return circumcircle(triangle).radius;
}
