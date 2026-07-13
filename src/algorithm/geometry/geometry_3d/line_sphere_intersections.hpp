#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "line3_direction.hpp"
#include "norm.hpp"
#include "projection.hpp"
#include "unit.hpp"

inline std::vector<Point3> line_sphere_intersections(
    const Line3& line,
    const Sphere3& sphere
){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const Point3 direction = line3_direction(line);
    const Point3 base = projection(line, sphere.center);
    const long double squared_height =
        sphere.radius * sphere.radius - norm(base - sphere.center);
    const int sign = geometry3d_sign(squared_height);
    if(sign < 0) return {};
    if(sign == 0) return {base};
    const Point3 offset = unit(direction) * std::sqrt(std::max(0.0L, squared_height));
    return {base - offset, base + offset};
}
