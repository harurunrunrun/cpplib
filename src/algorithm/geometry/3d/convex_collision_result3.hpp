#pragma once

#include <cstddef>

#include "base.hpp"

struct ConvexCollisionResult3{
    bool intersects = false;
    long double distance = 0.0L;
    long double penetration_depth = 0.0L;
    Point3 normal{};
    Point3 point_on_first{};
    Point3 point_on_second{};
    std::size_t gjk_iterations = 0;
    std::size_t epa_iterations = 0;
    bool converged = false;
};
