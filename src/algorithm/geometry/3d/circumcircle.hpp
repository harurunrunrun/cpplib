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
#include "unit.hpp"

inline Circle3 circumcircle(const Triangle3& triangle){
    const Point3 first = triangle.b - triangle.a;
    const Point3 second = triangle.c - triangle.a;
    const Point3 normal = cross(first, second);
    const long double squared_normal = norm(normal);
    if(geometry3d_sign(squared_normal) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const Point3 offset = (
        norm(first) * cross(second, normal) +
        norm(second) * cross(normal, first)
    ) / (2 * squared_normal);
    const Point3 center = triangle.a + offset;
    return {center, unit(normal), abs(offset)};
}
