#pragma once

#include <vector>

#include "../constants.hpp"
#include "../cross.hpp"
#include "../dot.hpp"

enum HalfPlaneIntersectionKind{
    HALF_PLANE_EMPTY,
    HALF_PLANE_BOUNDED,
    HALF_PLANE_UNBOUNDED,
};

struct HalfPlaneIntersectionResult{
    HalfPlaneIntersectionKind kind = HALF_PLANE_EMPTY;
    std::vector<Point> polygon;
};
