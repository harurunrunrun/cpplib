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
#include "geometry3d_sign.hpp"
#include "norm.hpp"
#include "scalar_triple.hpp"

inline Sphere3 circumsphere(const Tetrahedron3& tetrahedron){
    const Point3 first = tetrahedron.b - tetrahedron.a;
    const Point3 second = tetrahedron.c - tetrahedron.a;
    const Point3 third = tetrahedron.d - tetrahedron.a;
    const long double denominator = 2 * scalar_triple(first, second, third);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const Point3 offset = (
        norm(first) * cross(second, third) +
        norm(second) * cross(third, first) +
        norm(third) * cross(first, second)
    ) / denominator;
    const Point3 center = tetrahedron.a + offset;
    return {center, abs(offset)};
}
