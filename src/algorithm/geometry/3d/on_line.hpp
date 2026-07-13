#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "line3_direction.hpp"
#include "parallel.hpp"

inline bool on_line(const Line3& line, const Point3& point){
    return parallel(line3_direction(line), point - line.a);
}
