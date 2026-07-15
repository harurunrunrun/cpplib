#pragma once

#include "base.hpp"
#include "cross.hpp"

inline Point3 triangle_area_vector(const Triangle3& triangle){
    return cross(triangle.b - triangle.a, triangle.c - triangle.a) / 2;
}
