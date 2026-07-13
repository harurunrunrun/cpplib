#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "scalar_triple.hpp"

inline long double signed_volume(const Tetrahedron3& tetrahedron){
    return scalar_triple(
        tetrahedron.b - tetrahedron.a,
        tetrahedron.c - tetrahedron.a,
        tetrahedron.d - tetrahedron.a
    ) / 6;
}
