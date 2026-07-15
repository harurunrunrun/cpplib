#pragma once

#include "base.hpp"
#include "circumcircle.hpp"

inline Point3 triangle_circumcenter(const Triangle3& triangle){
    return circumcircle(triangle).center;
}
