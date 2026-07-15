#pragma once

#include <cstddef>

#include "base.hpp"

struct SATResult3{
    bool intersects = false;
    long double separation = 0.0L;
    long double penetration_depth = 0.0L;
    Point3 axis{};
    std::size_t tested_axes = 0;
};
