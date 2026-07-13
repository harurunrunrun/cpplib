#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "cross.hpp"

inline long double area(const Triangle3& triangle){
    return abs(cross(triangle.b - triangle.a, triangle.c - triangle.a)) / 2;
}
