#pragma once

#include <variant>

#include "base.hpp"
#include "polygon3.hpp"

using PlanarIntersection3 =
    std::variant<std::monostate, Point3, Segment3, Polygon3>;
