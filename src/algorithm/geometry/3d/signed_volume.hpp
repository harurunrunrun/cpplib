#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SIGNED_VOLUME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SIGNED_VOLUME_HPP_INCLUDED

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SIGNED_VOLUME_HPP_INCLUDED
