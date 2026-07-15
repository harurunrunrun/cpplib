#pragma once

#include <cstddef>
#include <vector>

#include "base.hpp"

struct GJKSupportPoint3{
    Point3 point{};
    Point3 point_on_first{};
    Point3 point_on_second{};
};

struct GJKResult3{
    bool intersects = false;
    long double distance = 0.0L;
    Point3 point_on_first{};
    Point3 point_on_second{};
    Point3 normal{};
    std::vector<GJKSupportPoint3> simplex;
    std::size_t iterations = 0;
    bool converged = false;
};
