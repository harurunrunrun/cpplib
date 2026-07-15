#pragma once

#include "base.hpp"
#include "closest_point.hpp"

inline Point3 projection(const Segment3& segment, const Point3& point){
    return closest_point(segment, point);
}
