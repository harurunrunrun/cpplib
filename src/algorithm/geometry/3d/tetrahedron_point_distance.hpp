#pragma once

#include "abs.hpp"
#include "base.hpp"
#include "tetrahedron_closest_point.hpp"

inline long double distance(
    const Tetrahedron3& tetrahedron,
    const Point3& point
){
    return abs(point - closest_point(tetrahedron, point));
}
