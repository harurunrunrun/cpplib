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
#include "signed_volume.hpp"

inline long double volume(const Tetrahedron3& tetrahedron){
    return std::abs(signed_volume(tetrahedron));
}
