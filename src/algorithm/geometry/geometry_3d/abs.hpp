#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "norm.hpp"

inline long double abs(const Point3& point){ return std::sqrt(norm(point)); }
