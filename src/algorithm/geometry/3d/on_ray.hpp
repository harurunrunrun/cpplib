#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "parallel.hpp"
#include "ray3_direction.hpp"

inline bool on_ray(const Ray3& ray, const Point3& point){
    const Point3 direction = ray3_direction(ray);
    return parallel(direction, point - ray.origin) &&
        geometry3d_sign(dot(direction, point - ray.origin)) >= 0;
}
