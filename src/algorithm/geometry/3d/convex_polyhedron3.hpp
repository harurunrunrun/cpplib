#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include "base.hpp"

struct ConvexPolyhedron3{
    int affine_dimension = -1;
    std::vector<Point3> vertices;
    std::vector<std::array<std::size_t, 3>> faces;
};
