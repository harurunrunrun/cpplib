#pragma once

#include <variant>

#include "base.hpp"

using LinearIntersection3 =
    std::variant<std::monostate, Point3, Segment3, Ray3, Line3>;
